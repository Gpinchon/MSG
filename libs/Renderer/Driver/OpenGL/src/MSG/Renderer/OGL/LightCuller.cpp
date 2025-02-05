#include <MSG/Camera.hpp>
#include <MSG/ECS/Registry.hpp>
#include <MSG/Entity/Camera.hpp>
#include <MSG/Light/PunctualLight.hpp>
#include <MSG/OGLBuffer.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLProgram.hpp>
#include <MSG/Renderer/OGL/Components/LightData.hpp>
#include <MSG/Renderer/OGL/LightCuller.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/ShaderCompiler.hpp>
#include <MSG/Scene.hpp>
#include <MSG/Transform.hpp>

#include <VTFS.glsl>

#include <GL/glew.h>

namespace MSG::Renderer {
template <typename LightType>
inline void MSG::Renderer::LightCuller::_PushLight(const LightType& a_Light)
{
    auto& lights = vtfs.lightsBufferCPU;
    if (lights.count >= VTFS_BUFFER_MAX) [[unlikely]]
        return;
    lights.lights[lights.count] = *reinterpret_cast<const GLSL::LightBase*>(&a_Light);
    lights.count++;
}

template <>
inline void MSG::Renderer::LightCuller::_PushLight(const Component::LightIBLData& a_Light)
{
    if (ibl.lights.size() >= _maxIBL) [[unlikely]]
        return;
    auto& glslLight         = ibl.lights.emplace_back();
    glslLight.commonData    = a_Light.commonData;
    glslLight.halfSize      = a_Light.halfSize;
    glslLight.specularIndex = ibl.samplers.size();
    for (auto i = 0; i < a_Light.irradianceCoefficients.size(); i++)
        glslLight.irradianceCoefficients[i] = glm::vec4(a_Light.irradianceCoefficients[i], 0);
    ibl.samplers.emplace_back(a_Light.specular);
}

template <>
inline void MSG::Renderer::LightCuller::_PushLight(const Component::LightData& a_LightData)
{
    return std::visit([this](auto& a_Data) mutable {
        _PushLight(a_Data);
    },
        a_LightData);
}

LightCullerVTFSBuffers::LightCullerVTFSBuffers(OGLContext& a_Ctx)
    : lightsBuffer(std::make_shared<OGLBuffer>(a_Ctx, sizeof(GLSL::VTFSLightsBuffer), nullptr, GL_DYNAMIC_STORAGE_BIT))
    , cluster(std::make_shared<OGLBuffer>(a_Ctx, sizeof(GLSL::VTFSCluster) * VTFS_CLUSTER_COUNT, GLSL::GenerateVTFSClusters().data(), GL_NONE))
{
}

LightCuller::LightCuller(Renderer::Impl& a_Renderer, const uint32_t& a_MaxIBL, const uint32_t& a_MaxShadowCasters)
    : _context(a_Renderer.context)
    , _maxIBL(a_MaxIBL)
    , _maxShadowCasters(a_MaxShadowCasters)
    , _vtfsCullingProgram(a_Renderer.shaderCompiler.CompileProgram("VTFSCulling"))
    , vtfs(_vtfs.front())
    , ibl(_ibl.front())
{
}

void LightCuller::operator()(Scene* a_Scene, const std::shared_ptr<OGLBuffer>& a_CameraUBO)
{
    vtfs         = _vtfs.at(_currentLightBuffer);
    ibl          = _ibl.at(_currentLightBuffer);
    auto& lights = vtfs.lightsBufferCPU;
    lights.count = 0;
    ibl.clear();
    for (auto& entity : a_Scene->GetVisibleEntities().lights) {
        _PushLight(entity.GetComponent<Component::LightData>());
    }
    _context.PushCmd([cameraUBO          = a_CameraUBO,
                         cullingProgram  = _vtfsCullingProgram,
                         GPUlightsBuffer = vtfs.lightsBuffer,
                         GPUclusters     = vtfs.cluster,
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
    _currentLightBuffer = (++_currentLightBuffer) % _vtfs.size();
}

}
