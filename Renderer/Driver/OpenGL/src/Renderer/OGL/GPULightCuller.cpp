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
static GLSL::LightBase ConvertLight(const GLSL::LightPoint& a_Light, const unsigned& a_IBLightIndex)
{
    return *reinterpret_cast<const GLSL::LightBase*>(&a_Light);
}

static GLSL::LightBase ConvertLight(const GLSL::LightSpot& a_Light, const unsigned& a_IBLightIndex)
{
    return *reinterpret_cast<const GLSL::LightBase*>(&a_Light);
}

static GLSL::LightBase ConvertLight(const GLSL::LightDirectional& a_Light, const unsigned& a_IBLightIndex)
{
    return *reinterpret_cast<const GLSL::LightBase*>(&a_Light);
}

static GLSL::LightBase ConvertLight(const Component::LightIBLData& a_Light, const unsigned& a_IBLightIndex)
{
    GLSL::LightIBL glslLight {};
    glslLight.commonData    = a_Light.commonData;
    glslLight.halfSize      = a_Light.halfSize;
    glslLight.specularIndex = a_IBLightIndex;
    for (auto i = 0; i < a_Light.irradianceCoefficients.size(); i++)
        glslLight.irradianceCoefficients[i] = glm::vec4(a_Light.irradianceCoefficients[i], 0);
    return *reinterpret_cast<GLSL::LightBase*>(&glslLight);
}

static GLSL::LightBase ConvertLight(const Component::LightData& a_LightData, const unsigned& a_IBLightIndex)
{
    return std::visit([a_IBLightIndex](auto& a_Data) {
        return ConvertLight(a_Data, a_IBLightIndex);
    },
        a_LightData);
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
    // pre-cull lights
    unsigned IBlLightCount = 0;
    lights.count           = 0;
    for (auto& entity : a_Scene->GetVisibleEntities()) {
        if (!entity.HasComponent<Component::LightData>())
            continue;
        auto& lightData = entity.GetComponent<Component::LightData>();
        if (lightData.GetType() == LIGHT_TYPE_IBL && IBlLightCount == VTFS_IBL_MAX)
            continue;
        auto worldLight = ConvertLight(lightData, IBlLightCount);
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
