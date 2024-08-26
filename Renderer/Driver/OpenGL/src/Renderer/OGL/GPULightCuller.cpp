#include <ECS/Registry.hpp>
#include <Renderer/OGL/Components/LightData.hpp>
#include <Renderer/OGL/GPULightCuller.hpp>
#include <Renderer/OGL/RAII/Buffer.hpp>
#include <Renderer/OGL/RAII/Program.hpp>
#include <Renderer/OGL/RAII/Wrapper.hpp>
#include <Renderer/OGL/Renderer.hpp>
#include <Renderer/OGL/ShaderCompiler.hpp>
#include <SG/Component/Camera.hpp>
#include <SG/Component/Light/PunctualLight.hpp>
#include <SG/Component/Transform.hpp>
#include <SG/Entity/Camera.hpp>
#include <SG/Scene/Scene.hpp>

#ifdef WIN32
#include <Renderer/OGL/Win32/Context.hpp>
#elif defined __linux__
#include <Renderer/OGL/Unix/Context.hpp>
#endif // WIN32

#include <VTFS.glsl>

#include <GL/glew.h>

namespace TabGraph::Renderer {
GLSL::LightBase GetGLSLLight(
    const Component::LightData& a_LightData,
    const unsigned& a_IBLightIndex)
{
    GLSL::LightBase glslLight;
    std::visit([&glslLight](auto& a_Data) {
        glslLight.commonData = a_Data.commonData;
    },
        a_LightData);
    switch (a_LightData.GetType()) {
    // case LIGHT_TYPE_POINT: {
    //     auto& dirLight = reinterpret_cast<GLSL::LightPoint&>(glslLight);
    //     dirLight       = std::get<GLSL::LightPoint>(a_LightData);
    // } break;
    case LIGHT_TYPE_DIRECTIONAL: {
        auto& dirLight = reinterpret_cast<GLSL::LightDirectional&>(glslLight);
        dirLight       = std::get<GLSL::LightDirectional>(a_LightData);
    } break;
    case LIGHT_TYPE_SPOT: {
        auto& spot = reinterpret_cast<GLSL::LightSpot&>(glslLight);
        spot       = std::get<GLSL::LightSpot>(a_LightData);
    } break;
    case LIGHT_TYPE_IBL: {
        auto& IBL         = reinterpret_cast<GLSL::LightIBL&>(glslLight);
        auto& IBLData     = std::get<Component::LightIBLData>(a_LightData);
        IBL.specularIndex = a_IBLightIndex;
        for (auto i = 0; i < IBLData.irradianceCoefficients.size(); i++)
            IBL.irradianceCoefficients[i] = glm::vec4(IBLData.irradianceCoefficients[i], 0);
    } break;
    default:
        break;
    }
    return glslLight;
}

bool LightIntersects(
    IN(GLSL::LightBase) a_Light,
    IN(glm::mat4x4) a_MVP,
    IN(glm::vec3) a_AABBMin,
    IN(glm::vec3) a_AABBMax)
{
    if (a_Light.commonData.type == LIGHT_TYPE_POINT || a_Light.commonData.type == LIGHT_TYPE_SPOT) {
        glm::vec3 lightPosition = a_Light.commonData.position;
        float lightRadius       = a_Light.commonData.range;
        GLSL::ProjectSphereToNDC(lightPosition, lightRadius, a_MVP);
        // closest point on the AABB to the sphere center
        glm::vec3 closestPoint = glm::clamp(lightPosition, a_AABBMin, a_AABBMax);
        glm::vec3 diff         = closestPoint - lightPosition;
        // squared distance between the sphere center and closest point
        if (float distanceSquared = dot(diff, diff); !(distanceSquared <= lightRadius * lightRadius))
            return false;
        // if it's a spot and it's outside the AABB
        if (a_Light.commonData.type == LIGHT_TYPE_SPOT && closestPoint != lightPosition) {
            auto& spot                  = reinterpret_cast<const GLSL::LightSpot&>(a_Light);
            glm::vec3 ndcLightDirection = glm::normalize(a_MVP * glm::vec4(spot.direction, 0));
            glm::vec3 closestDir        = glm::normalize(lightPosition - closestPoint);
            float closestAngle          = glm::dot(closestDir, ndcLightDirection);
            return closestAngle <= spot.outerConeAngle;
        }
        return true;
    } else if (a_Light.commonData.type == LIGHT_TYPE_IBL) {
        return true;
    }
    return false;
}

GPULightCuller::GPULightCuller(Renderer::Impl& a_Renderer)
    : _renderer(a_Renderer)
    , _cullingProgram(a_Renderer.shaderCompiler.CompileProgram("VTFSCulling"))
{

    for (uint32_t i = 0; i < GPULightCullerBufferNbr; i++) {
        _GPUclustersBuffers.at(i) = RAII::MakePtr<RAII::Buffer>(_renderer.context, sizeof(GLSL::VTFSCluster) * VTFS_CLUSTER_COUNT, GLSL::GenerateVTFSClusters().data(), GL_NONE);
        _GPUlightsBuffers.at(i)   = RAII::MakePtr<RAII::Buffer>(_renderer.context, sizeof(GLSL::VTFSLightsBuffer), nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
        a_Renderer.context.PushImmediateCmd([this, lightBuffer = _GPUlightsBuffers.at(i), i] {
            auto bufferPtr             = glMapNamedBufferRange(*lightBuffer, 0, lightBuffer->size, GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);
            _GPULightsBufferPtrs.at(i) = reinterpret_cast<GLSL::VTFSLightsBuffer*>(bufferPtr);
        });
    }
    GPUlightsBuffer = _GPUlightsBuffers.at(0);
    GPUclusters     = _GPUclustersBuffers.at(0);
}

void GPULightCuller::operator()(SG::Scene* a_Scene)
{
    GPUlightsBuffer = _GPUlightsBuffers.at(_currentLightBuffer);
    GPUclusters     = _GPUclustersBuffers.at(_currentLightBuffer);
    iblSamplers.fill(nullptr);
    auto& lights    = *_GPULightsBufferPtrs.at(_currentLightBuffer);
    auto registry   = a_Scene->GetRegistry();
    auto cameraView = SG::Camera::GetViewMatrix(a_Scene->GetCamera());
    auto cameraProj = a_Scene->GetCamera().GetComponent<SG::Component::Camera>().projection.GetMatrix();
    auto MVP        = cameraProj * cameraView;
    GLSL::VTFSClusterAABB cameraFrustum;
    cameraFrustum.minPoint = { -1, -1, -1 };
    cameraFrustum.maxPoint = { 1, 1, 1 };
    auto lightView         = registry->GetView<Component::LightData>();
    // pre-cull lights
    unsigned IBlLightCount = 0;
    lights.count           = 0;
    for (const auto& [entityID, lightData] : lightView) {
        if (lightData.GetType() == LIGHT_TYPE_IBL && IBlLightCount == VTFS_IBL_MAX)
            continue;
        auto entity     = registry->GetEntityRef(entityID);
        auto worldLight = GetGLSLLight(lightData, IBlLightCount);
        if (LightIntersects(worldLight, MVP, cameraFrustum.minPoint, cameraFrustum.maxPoint)) {
            if (lightData.GetType() == LIGHT_TYPE_IBL) {
                reinterpret_cast<GLSL::LightIBL&>(worldLight).specularIndex = IBlLightCount;
                iblSamplers[IBlLightCount]                                  = std::get<Component::LightIBLData>(lightData).specular;
                IBlLightCount++;
            }
            lights.lights[lights.count] = worldLight;
            lights.count++;
            if (lights.count == VTFS_BUFFER_MAX)
                break;
        }
    }
    _renderer.context.PushCmd([cameraUBO          = _renderer.cameraUBO.buffer,
                                  cullingProgram  = _cullingProgram,
                                  GPUlightsBuffer = GPUlightsBuffer,
                                  GPUclusters     = GPUclusters,
                                  lightCount      = lights.count] {
        auto lightBufferFlushSize = offsetof(GLSL::VTFSLightsBuffer, lights) + (sizeof(GLSL::LightBase) * lightCount);
        glFlushMappedNamedBufferRange(*GPUlightsBuffer, 0, lightBufferFlushSize);
        glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, *cameraUBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, *GPUlightsBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, *GPUclusters);
        glUseProgram(*cullingProgram);
        glDispatchCompute(VTFS_CLUSTER_COUNT / VTFS_LOCAL_SIZE, 1, 1);
        glMemoryBarrierByRegion(GL_SHADER_STORAGE_BARRIER_BIT);
        //   unbind objects
        glUseProgram(0);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
    });
    _currentLightBuffer = (++_currentLightBuffer) % _GPUlightsBuffers.size();
}

}
