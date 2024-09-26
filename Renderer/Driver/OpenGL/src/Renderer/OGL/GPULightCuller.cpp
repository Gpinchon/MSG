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

#ifdef _WIN32
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
    case LIGHT_TYPE_POINT: {
        auto& point             = reinterpret_cast<GLSL::LightPoint&>(glslLight);
        point                   = std::get<GLSL::LightPoint>(a_LightData);
        point.commonData.radius = point.range;
    } break;
    case LIGHT_TYPE_DIRECTIONAL: {
        auto& dir             = reinterpret_cast<GLSL::LightDirectional&>(glslLight);
        dir                   = std::get<GLSL::LightDirectional>(a_LightData);
        dir.commonData.radius = glm::length(dir.halfSize);
    } break;
    case LIGHT_TYPE_SPOT: {
        auto& spot             = reinterpret_cast<GLSL::LightSpot&>(glslLight);
        spot                   = std::get<GLSL::LightSpot>(a_LightData);
        spot.commonData.radius = spot.range;
    } break;
    case LIGHT_TYPE_IBL: {
        auto& IBL             = reinterpret_cast<GLSL::LightIBL&>(glslLight);
        auto& IBLData         = std::get<Component::LightIBLData>(a_LightData);
        IBL.halfSize          = IBLData.halfSize;
        IBL.specularIndex     = a_IBLightIndex;
        IBL.commonData.radius = glm::length(IBLData.halfSize);
        for (auto i = 0; i < IBLData.irradianceCoefficients.size(); i++)
            IBL.irradianceCoefficients[i] = glm::vec4(IBLData.irradianceCoefficients[i], 0);
    } break;
    default:
        break;
    }
    return glslLight;
}

GPULightCuller::GPULightCuller(Renderer::Impl& a_Renderer)
    : _renderer(a_Renderer)
    , _cullingProgram(a_Renderer.shaderCompiler.CompileProgram("VTFSCulling"))
{

    for (uint32_t i = 0; i < GPULightCullerBufferNbr; i++) {
        _GPUclustersBuffers.at(i) = RAII::MakePtr<RAII::Buffer>(_renderer.context, sizeof(GLSL::VTFSCluster) * VTFS_CLUSTER_COUNT, GLSL::GenerateVTFSClusters().data(), GL_NONE);
        _GPUlightsBuffers.at(i)   = RAII::MakePtr<RAII::Buffer>(_renderer.context, sizeof(GLSL::VTFSLightsBuffer), nullptr, GL_DYNAMIC_STORAGE_BIT);
    }
    GPUlightsBuffer = _GPUlightsBuffers.at(0);
    GPUclusters     = _GPUclustersBuffers.at(0);
}

void GPULightCuller::operator()(SG::Scene* a_Scene)
{
    GPUlightsBuffer = _GPUlightsBuffers.at(_currentLightBuffer);
    GPUclusters     = _GPUclustersBuffers.at(_currentLightBuffer);
    iblSamplers.fill(nullptr);
    auto& lights    = _LightsBuffer.at(_currentLightBuffer);
    auto& registry  = a_Scene->GetRegistry();
    auto cameraView = SG::Camera::GetViewMatrix(a_Scene->GetCamera());
    auto cameraProj = a_Scene->GetCamera().GetComponent<SG::Component::Camera>().projection.GetMatrix();
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
        if (LightIntersectsAABB(worldLight, cameraView, cameraProj, cameraFrustum.minPoint, cameraFrustum.maxPoint)) {
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
                                  &lights         = lights] {
        auto lightBufferSize = offsetof(GLSL::VTFSLightsBuffer, lights) + (sizeof(GLSL::LightBase) * lights.count);
        // upload data
        glInvalidateBufferSubData(*GPUlightsBuffer, 0, lightBufferSize);
        glNamedBufferSubData(*GPUlightsBuffer, 0, lightBufferSize, &lights);
        // bind objects
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, *cameraUBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, *GPUlightsBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, *GPUclusters);
        glUseProgram(*cullingProgram);
        // dispatch compute
        glDispatchCompute(VTFS_CLUSTER_COUNT / VTFS_LOCAL_SIZE, 1, 1);
        glMemoryBarrierByRegion(GL_SHADER_STORAGE_BARRIER_BIT);
        // unbind objects
        glUseProgram(0);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
    });
    _currentLightBuffer = (++_currentLightBuffer) % _GPUlightsBuffers.size();
}

}
