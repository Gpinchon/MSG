#include <MSG/Camera.hpp>
#include <MSG/ECS/Registry.hpp>
#include <MSG/Entity/Camera.hpp>
#include <MSG/Light/PunctualLight.hpp>

#include <MSG/Renderer/OGL/Renderer.hpp>

#include <MSG/Renderer/OGL/Components/LightData.hpp>

#include <MSG/Renderer/OGL/Subsystems/CameraSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/FrameSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/LightsSubsystem.hpp>

#include <MSG/OGLPipeline.hpp>
#include <MSG/OGLSampler.hpp>

#include <LightsIBLInputs.glsl>
#include <LightsShadowInputs.glsl>
#include <LightsVTFS.glsl>

#include <GL/glew.h>

INLINE auto GetShadowSamplerParameters()
{
    Msg::OGLSamplerParameters parameters;
    parameters.minFilter   = GL_LINEAR;
    parameters.wrapS       = GL_CLAMP_TO_BORDER;
    parameters.wrapT       = GL_CLAMP_TO_BORDER;
    parameters.wrapR       = GL_CLAMP_TO_BORDER;
    parameters.compareMode = GL_COMPARE_REF_TO_TEXTURE;
    parameters.compareFunc = GL_LEQUAL;
    parameters.borderColor = glm::vec4(1);
    return parameters;
}

INLINE auto GetShadowSamplerCubeParameters()
{
    Msg::OGLSamplerParameters parameters;
    parameters.minFilter   = GL_LINEAR;
    parameters.wrapS       = GL_CLAMP_TO_EDGE;
    parameters.wrapT       = GL_CLAMP_TO_EDGE;
    parameters.wrapR       = GL_CLAMP_TO_EDGE;
    parameters.compareMode = GL_COMPARE_REF_TO_TEXTURE;
    parameters.compareFunc = GL_LEQUAL;
    return parameters;
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

Msg::Renderer::LightsVTFSBuffer::LightsVTFSBuffer(Msg::OGLContext& a_Ctx)
    : lightsBuffer(std::make_shared<OGLTypedBuffer<GLSL::VTFSLightsBuffer>>(a_Ctx))
    , cluster(std::make_shared<OGLTypedBufferArray<GLSL::VTFSCluster>>(a_Ctx, VTFS_CLUSTER_COUNT, GenerateVTFSClusters().data()))
    , cmdBuffer(a_Ctx)
{
}

Msg::Renderer::LightsVTFS::LightsVTFS(Renderer::Impl& a_Renderer)
    : _cullingProgram(a_Renderer.shaderCompiler.CompileProgram("VTFSCulling"))
    , _buffers({ { a_Renderer.context }, { a_Renderer.context } })
    , buffer(&_buffers.front())
{
}

void Msg::Renderer::LightsVTFS::Prepare()
{
    buffer = &_buffers.at(_currentBuffer);
}

void Msg::Renderer::LightsVTFS::Update(const SubsystemsLibrary& a_Subsystems)
{
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

Msg::Renderer::LightsIBL::LightsIBL(OGLContext& a_Ctx)
    : buffer(std::make_shared<OGLTypedBuffer<GLSL::LightsIBLUBO>>(a_Ctx))
{
}

Msg::Renderer::LightsShadows::LightsShadows(OGLContext& a_Ctx)
    : dataBuffer(std::make_shared<OGLTypedBuffer<GLSL::ShadowsBase>>(a_Ctx))
    , viewportsBuffer(std::make_shared<OGLTypedBufferArray<GLSL::Camera>>(a_Ctx, 32))
{
}

template <typename LightType>
inline void Msg::Renderer::LightsSubsystem::_PushLight(
    const LightType& a_LightData,
    GLSL::VTFSLightsBuffer& a_VTFS,
    GLSL::LightsIBLUBO&,
    GLSL::ShadowsBase&,
    GLSL::Camera (&)[SHADOW_MAX_VIEWPORTS],
    size_t&,
    const size_t&)
{
    if (a_VTFS.count >= VTFS_BUFFER_MAX) [[unlikely]]
        return;
    a_VTFS.lights[a_VTFS.count] = *reinterpret_cast<const GLSL::LightBase*>(&a_LightData);
    a_VTFS.count++;
}

template <>
inline void Msg::Renderer::LightsSubsystem::_PushLight(
    const Renderer::LightIBLData& a_LightData,
    GLSL::VTFSLightsBuffer&,
    GLSL::LightsIBLUBO& a_IBL,
    GLSL::ShadowsBase&,
    GLSL::Camera (&)[SHADOW_MAX_VIEWPORTS],
    size_t&,
    const size_t&)
{
    if (a_IBL.count < SAMPLERS_IBL_COUNT) [[likely]] {
        auto& index       = a_IBL.count;
        auto& ibl         = a_IBL.lights[index];
        ibl.commonData    = a_LightData.commonData;
        ibl.boxProjection = a_LightData.boxProjection;
        ibl.halfSize      = a_LightData.halfSize;
        for (uint8_t i = 0; i < a_LightData.irradianceCoefficients.size(); i++)
            ibl.irradianceCoefficients[i] = GLSL::vec4(a_LightData.irradianceCoefficients[i], 1.f);
        ibls.textures[index] = a_LightData.specular;
        a_IBL.count++;
    }
}

template <>
inline void Msg::Renderer::LightsSubsystem::_PushLight(
    const Renderer::LightData& a_LightData,
    GLSL::VTFSLightsBuffer& a_VTFS,
    GLSL::LightsIBLUBO& a_IBL,
    GLSL::ShadowsBase& a_Shadows,
    GLSL::Camera (&a_Viewports)[SHADOW_MAX_VIEWPORTS],
    size_t& a_ViewportIndex,
    const size_t& a_MaxShadows)
{
    if (a_LightData.shadow.has_value() && a_Shadows.count < a_MaxShadows && a_Shadows.count < SAMPLERS_SHADOW_COUNT) [[unlikely]] {
        auto& index     = a_Shadows.count;
        auto& shadow    = a_Shadows.shadows[index];
        auto& glslLight = *std::visit(
            [](auto& a_Data) {
                return reinterpret_cast<const GLSL::LightBase*>(&a_Data);
            },
            a_LightData);
        const auto& depthRange = a_LightData.shadow->depthRanges[a_LightData.shadow->depthRangeIndex_Prev];
        shadow.light           = glslLight;
        shadow.blurRadius      = a_LightData.shadow->blurRadius;
        shadow.bias            = a_LightData.shadow->bias;
        shadow.normalBias      = a_LightData.shadow->normalBias;
        shadow.minDepth        = depthRange->Get(0);
        shadow.maxDepth        = depthRange->Get(1);
        shadow.viewportIndex   = a_ViewportIndex;
        shadow.viewportCount   = a_LightData.shadow->projections.size();
        for (auto& proj : a_LightData.shadow->projections) {
            a_Viewports[a_ViewportIndex] = proj;
            a_ViewportIndex++;
        }
        shadows.texturesDepth[index] = a_LightData.shadow->textureDepth;
        a_Shadows.count++;
        return;
    }
    return std::visit([this, &a_VTFS, &a_IBL, &a_Shadows, &a_Viewports, &a_ViewportIndex, &a_MaxShadows](auto& a_Data) mutable { _PushLight(a_Data, a_VTFS, a_IBL, a_Shadows, a_Viewports, a_ViewportIndex, a_MaxShadows); }, a_LightData);
}

Msg::Renderer::LightsSubsystem::LightsSubsystem(Renderer::Impl& a_Renderer)
    : SubsystemInterface({
          typeid(CameraSubsystem),
          typeid(FrameSubsystem),
      })
    , vtfs(a_Renderer)
    , ibls(a_Renderer.context)
    , shadows(a_Renderer.context)
    , iblSpecSampler(std::make_shared<OGLSampler>(a_Renderer.context, OGLSamplerParameters { .minFilter = GL_LINEAR_MIPMAP_LINEAR }))
    , shadowSampler(std::make_shared<OGLSampler>(a_Renderer.context, GetShadowSamplerParameters()))
    , shadowSamplerCube(std::make_shared<OGLSampler>(a_Renderer.context, GetShadowSamplerCubeParameters()))
{
}

void Msg::Renderer::LightsSubsystem::Load(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    if (a_Entity.HasComponent<PunctualLight>() && !a_Entity.HasComponent<Renderer::LightData>())
        a_Entity.AddComponent<Renderer::LightData>(a_Renderer, *a_Entity.GetRegistry(), a_Entity);
}

void Msg::Renderer::LightsSubsystem::Unload(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    if (a_Entity.HasComponent<Renderer::LightData>())
        a_Entity.RemoveComponent<Renderer::LightData>();
}

void Msg::Renderer::LightsSubsystem::Update(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems)
{
    auto& activeScene          = a_Renderer.activeScene;
    auto& registry             = *activeScene->GetRegistry();
    const auto& visibleLights  = activeScene->GetVisibleEntities().lights;
    const auto& visibleShadows = activeScene->GetVisibleEntities().shadows;

    std::scoped_lock lock(registry.GetLock());
    for (auto& shadow : visibleShadows) {
        auto& lightData = registry.GetComponent<Renderer::LightData>(shadow);
        lightData.shadow->Update(a_Renderer, registry, shadow, shadow.viewports);
    }
    for (auto& entityID : visibleLights) {
        auto& lightData = registry.GetComponent<Renderer::LightData>(entityID);
        lightData.Update(a_Renderer, registry, entityID);
    }
    vtfs.Prepare();

    // Update OGL buffers
    GLSL::VTFSLightsBuffer vtfsBuffer = vtfs.buffer->lightsBuffer->Get();
    GLSL::LightsIBLUBO iblBuffer      = ibls.buffer->Get();
    GLSL::ShadowsBase shadowsData     = shadows.dataBuffer->Get();
    GLSL::Camera shadowViewports[SHADOW_MAX_VIEWPORTS];
    size_t shadowViewportsIndex = 0;
    vtfsBuffer.count            = 0;
    iblBuffer.count             = 0;
    shadowsData.count           = 0;
    for (auto& entity : activeScene->GetVisibleEntities().lights)
        _PushLight(registry.GetComponent<Renderer::LightData>(entity), vtfsBuffer, iblBuffer, shadowsData, shadowViewports, shadowViewportsIndex, visibleShadows.size());
    vtfs.buffer->lightsBuffer->Set(vtfsBuffer);
    ibls.buffer->Set(iblBuffer);
    shadows.dataBuffer->Set(shadowsData);
    shadows.viewportsBuffer->Set(0, shadowViewportsIndex, shadowViewports);

    vtfs.buffer->lightsBuffer->Update();
    ibls.buffer->Update();
    shadows.dataBuffer->Update();
    shadows.viewportsBuffer->Update();

    vtfs.Update(a_Subsystems);
}
