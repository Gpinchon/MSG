#include <MSG/Camera.hpp>
#include <MSG/ECS/Registry.hpp>
#include <MSG/Entity/Camera.hpp>
#include <MSG/Light/PunctualLight.hpp>
#include <MSG/Scene.hpp>

#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/Subsystems/CameraSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/FrameSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/LightsImageBasedSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/LightsShadowSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/LightsVTFSSubsystem.hpp>

#include <MSG/Renderer/OGL/Components/LightImageBasedData.hpp>
#include <MSG/Renderer/OGL/Components/LightShadowData.hpp>

#include <MSG/OGLPipeline.hpp>
#include <MSG/OGLSampler.hpp>
#include <MSG/OGLTexture.hpp>
#include <MSG/OGLTypedBuffer.hpp>

#include <LightsVTFS.glsl>

#include <GL/glew.h>

namespace Msg::Renderer {
union GLSLPunctualLight {
    GLSL::LightBase base;
    GLSL::LightPoint point;
    GLSL::LightSpot spot;
    GLSL::LightDirectional dir;
    GLSL::LightIBLIndex ibl;
};

static constexpr int GetGLSLLightType(const LightType& a_LightType)
{
    switch (a_LightType) {
    case LightType::Point:
        return LIGHT_TYPE_POINT;
    case LightType::Spot:
        return LIGHT_TYPE_SPOT;
    case LightType::Directional:
        return LIGHT_TYPE_DIRECTIONAL;
    case LightType::IBL:
        return LIGHT_TYPE_IBL;
    }
    return LIGHT_TYPE_UNKNOWN;
}

static GLSL::LightCommon GetGLSLCommonData(const PunctualLight& a_Light, const Transform& a_Transform, uint32_t& a_ShadowCasterIndex, uint32_t& a_IBLIndex)
{
    GLSL::LightCommon common;
    common.color               = a_Light.GetColor();
    common.falloff             = a_Light.GetFalloff();
    common.intensity           = a_Light.GetIntensity();
    common.lightShaftIntensity = a_Light.GetLightShaftIntensity();
    common.position            = a_Transform.GetWorldPosition();
    common.priority            = a_Light.GetPriority();
    common.radius              = a_Light.GetRadius();
    common.shadowCasterIndex   = -1; // -1 is no shadow caster
    common.type                = GetGLSLLightType(a_Light.GetType());
    if (a_Light.CastsShadow()) {
        common.shadowCasterIndex = a_ShadowCasterIndex;
        a_ShadowCasterIndex++;
    }
    return common;
}

static GLSLPunctualLight ConvertLight(const LightPoint& a_SGLight, const Msg::Transform& a_Transform, uint32_t& a_ShadowCasterIndex, uint32_t& a_IBLIndex)
{
    GLSLPunctualLight glslLight;
    glslLight.point.commonData = GetGLSLCommonData(a_SGLight, a_Transform, a_ShadowCasterIndex, a_IBLIndex);
    glslLight.point.range      = a_SGLight.range;
    return glslLight;
}

static GLSLPunctualLight ConvertLight(const LightSpot& a_SGLight, const Msg::Transform& a_Transform, uint32_t& a_ShadowCasterIndex, uint32_t& a_IBLIndex)
{
    GLSLPunctualLight glslLight;
    glslLight.spot.commonData     = GetGLSLCommonData(a_SGLight, a_Transform, a_ShadowCasterIndex, a_IBLIndex);
    glslLight.spot.range          = a_SGLight.range;
    glslLight.spot.direction      = a_Transform.GetWorldForward();
    glslLight.spot.innerConeAngle = a_SGLight.innerConeAngle;
    glslLight.spot.outerConeAngle = a_SGLight.outerConeAngle;
    return glslLight;
}

static GLSLPunctualLight ConvertLight(const LightDirectional& a_SGLight, const Msg::Transform& a_Transform, uint32_t& a_ShadowCasterIndex, uint32_t& a_IBLIndex)
{
    GLSLPunctualLight glslLight;
    glslLight.dir.commonData = GetGLSLCommonData(a_SGLight, a_Transform, a_ShadowCasterIndex, a_IBLIndex);
    glslLight.dir.halfSize   = a_SGLight.halfSize;
    glslLight.dir.direction  = a_Transform.GetWorldForward();
    return glslLight;
}

static GLSLPunctualLight ConvertLight(const LightIBL& a_SGLight, const Msg::Transform& a_Transform, uint32_t& a_ShadowCasterIndex, uint32_t& a_IBLIndex)
{
    GLSLPunctualLight glslLight;
    glslLight.ibl.commonData = GetGLSLCommonData(a_SGLight, a_Transform, a_ShadowCasterIndex, a_IBLIndex);
    glslLight.ibl.index      = a_IBLIndex; //-1 is no IBL sampler
    a_IBLIndex++;
    return glslLight;
}

/*
 * @brief VTFS clusters bounding box are generated only once and never change so they're only generated on the CPU
 */
INLINE std::vector<Msg::Renderer::GLSL::VTFSCluster> GenerateVTFSClusters()
{
    constexpr glm::vec3 clusterSize = {
        1.f / VTFS_CLUSTER_X,
        1.f / VTFS_CLUSTER_Y,
        1.f / VTFS_CLUSTER_Z,
    };
    std::vector<Msg::Renderer::GLSL::VTFSCluster> clusters(VTFS_CLUSTER_COUNT);
    for (size_t z = 0; z < VTFS_CLUSTER_Z; ++z) {
        for (size_t y = 0; y < VTFS_CLUSTER_Y; ++y) {
            for (size_t x = 0; x < VTFS_CLUSTER_X; ++x) {
                glm::vec3 NDCMin           = (glm::vec3(x, y, z) * clusterSize) * 2.f - 1.f;
                glm::vec3 NDCMax           = NDCMin + clusterSize * 2.f;
                auto lightClusterIndex     = Msg::Renderer::GLSL::VTFSClusterIndexTo1D({ x, y, z });
                auto& lightCluster         = clusters[lightClusterIndex];
                lightCluster.aabb.minPoint = Msg::Renderer::GLSL::VTFSClusterPosition(NDCMin);
                lightCluster.aabb.maxPoint = Msg::Renderer::GLSL::VTFSClusterPosition(NDCMax);
            }
        }
    }
    return clusters;
}
}

Msg::Renderer::LightsVTFSBuffer::LightsVTFSBuffer(Msg::OGLContext& a_Ctx)
    : lightsBuffer(std::make_shared<OGLTypedBuffer<GLSL::VTFSLightsBuffer>>(a_Ctx))
    , cluster(std::make_shared<OGLTypedBufferArray<GLSL::VTFSCluster>>(a_Ctx, VTFS_CLUSTER_COUNT, GenerateVTFSClusters().data()))
    , cmdBuffer(a_Ctx)
{
}

Msg::Renderer::LightsVTFSSubsystem::LightsVTFSSubsystem(Renderer::Impl& a_Renderer)
    : SubsystemInterface({
          typeid(CameraSubsystem),
          typeid(FrameSubsystem),
          typeid(LightsImageBasedSubsystem),
          typeid(LightsShadowSubsystem),
      })
    , _cullingProgram(a_Renderer.shaderCompiler.CompileProgram("VTFSCulling"))
    , _buffers({ { a_Renderer.context }, { a_Renderer.context } })
    , buffer(&_buffers.front())
{
}

void Msg::Renderer::LightsVTFSSubsystem::Load(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    // no VTFS specific component
}

void Msg::Renderer::LightsVTFSSubsystem::Unload(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    // no VTFS specific component
}

void Msg::Renderer::LightsVTFSSubsystem::Update(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems)
{
    auto& activeScene         = a_Renderer.activeScene;
    auto& registry            = *activeScene->GetRegistry();
    const auto& visibleLights = activeScene->GetVisibleEntities().lights;

    std::scoped_lock lock(registry.GetLock());
    buffer = &_buffers.at(_currentBuffer);
    // Update OGL buffers
    uint32_t shadowCount              = 0; // 1 because 0 is default shadow sampler
    uint32_t iblCount                 = 0;
    GLSL::VTFSLightsBuffer vtfsBuffer = buffer->lightsBuffer->Get();
    vtfsBuffer.count                  = 0;
    for (auto& entity : visibleLights) {
        auto entityRef      = registry.GetEntityRef(entity);
        auto& punctualLight = entityRef.GetComponent<PunctualLight>();
        auto& transform     = entityRef.GetComponent<Transform>();
        std::visit([&](auto& a_SGLight) mutable {
            GLSLPunctualLight glslLight         = ConvertLight(a_SGLight, transform, shadowCount, iblCount);
            vtfsBuffer.lights[vtfsBuffer.count] = *reinterpret_cast<const GLSL::LightBase*>(&glslLight);
            vtfsBuffer.count++;
        },
            punctualLight);
        if (vtfsBuffer.count >= VTFS_BUFFER_MAX) [[unlikely]]
            break;
    }
    buffer->lightsBuffer->Set(vtfsBuffer);
    buffer->lightsBuffer->Update();
    auto& cameraSubsystem = a_Subsystems.Get<CameraSubsystem>();
    OGLComputePipelineInfo cp;
    cp.bindings.uniformBuffers[UBO_CAMERA] = { .buffer = cameraSubsystem.buffer, .offset = 0, .size = cameraSubsystem.buffer->size };
    cp.bindings.storageBuffers[0]          = { .buffer = buffer->lightsBuffer, .offset = 0, .size = buffer->lightsBuffer->size };
    cp.bindings.storageBuffers[1]          = { .buffer = buffer->cluster, .offset = 0, .size = buffer->cluster->size };
    cp.shaderState.program                 = _cullingProgram;
    buffer->executionFence.Wait();
    buffer->executionFence.Reset();
    buffer->cmdBuffer.Reset();
    buffer->cmdBuffer.Begin();
    buffer->cmdBuffer.PushCmd<OGLCmdPushPipeline>(cp);
    buffer->cmdBuffer.PushCmd<OGLCmdDispatchCompute>(
        OGLCmdDispatchComputeInfo {
            .workgroupX = VTFS_CLUSTER_COUNT / VTFS_LOCAL_SIZE,
            .workgroupY = 1,
            .workgroupZ = 1,
        });
    buffer->cmdBuffer.PushCmd<OGLCmdMemoryBarrier>(GL_SHADER_STORAGE_BARRIER_BIT, true);
    buffer->cmdBuffer.End();
    buffer->cmdBuffer.Execute(&buffer->executionFence);
    _currentBuffer = (++_currentBuffer) % _buffers.size();
}
