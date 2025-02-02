#include <MSG/Camera.hpp>
#include <MSG/Transform.hpp>
#include <MSG/ECS/Registry.hpp>
#include <MSG/Entity/Camera.hpp>
#include <MSG/Light/PunctualLight.hpp>
#include <MSG/OGLBuffer.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLProgram.hpp>
#include <MSG/Renderer/OGL/Components/LightData.hpp>
#include <MSG/Renderer/OGL/GPULightCuller.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/ShaderCompiler.hpp>
#include <MSG/Scene.hpp>

#include <VTFS.glsl>

#include <GL/glew.h>

namespace MSG::Renderer {
template <typename LightType>
static GLSL::LightBase ConvertLight(const LightType& a_Light, std::array<std::shared_ptr<OGLTextureCubemap>, VTFS_IBL_MAX>&, unsigned&)
{
    return *reinterpret_cast<const GLSL::LightBase*>(&a_Light);
}

static GLSL::LightBase ConvertLight(const Component::LightIBLData& a_Light, std::array<std::shared_ptr<OGLTextureCubemap>, VTFS_IBL_MAX>& a_IBLSamplers, unsigned& a_IBLightIndex)
{
    GLSL::LightIBL glslLight {};
    glslLight.commonData    = a_Light.commonData;
    glslLight.halfSize      = a_Light.halfSize;
    glslLight.specularIndex = a_IBLightIndex;
    for (auto i = 0; i < a_Light.irradianceCoefficients.size(); i++)
        glslLight.irradianceCoefficients[i] = glm::vec4(a_Light.irradianceCoefficients[i], 0);
    a_IBLSamplers[a_IBLightIndex] = a_Light.specular;
    ++a_IBLightIndex;
    return *reinterpret_cast<GLSL::LightBase*>(&glslLight);
}

static GLSL::LightBase ConvertLight(const Component::LightData& a_LightData, std::array<std::shared_ptr<OGLTextureCubemap>, VTFS_IBL_MAX>& a_IBLSamplers, unsigned& a_IBLightIndex)
{
    return std::visit([&a_IBLSamplers, &a_IBLightIndex](auto& a_Data) mutable {
        return ConvertLight(a_Data, a_IBLSamplers, a_IBLightIndex);
    },
        a_LightData);
}

GPULightCuller::GPULightCuller(Renderer::Impl& a_Renderer)
    : _renderer(a_Renderer)
    , _cullingProgram(a_Renderer.shaderCompiler.CompileProgram("VTFSCulling"))
{
    for (uint32_t i = 0; i < GPULightCullerBufferNbr; i++) {
        _GPUclustersBuffers.at(i) = std::make_shared<OGLBuffer>(_renderer.context, sizeof(GLSL::VTFSCluster) * VTFS_CLUSTER_COUNT, GLSL::GenerateVTFSClusters().data(), GL_NONE);
        _GPUlightsBuffers.at(i)   = std::make_shared<OGLBuffer>(_renderer.context, sizeof(GLSL::VTFSLightsBuffer), nullptr, GL_DYNAMIC_STORAGE_BIT);
    }
    GPUlightsBuffer = _GPUlightsBuffers.at(0);
    GPUclusters     = _GPUclustersBuffers.at(0);
}

void GPULightCuller::operator()(Scene* a_Scene)
{
    iblSamplers.fill(nullptr);
    GPUlightsBuffer        = _GPUlightsBuffers.at(_currentLightBuffer);
    GPUclusters            = _GPUclustersBuffers.at(_currentLightBuffer);
    auto& lights           = _LightsBuffer.at(_currentLightBuffer);
    unsigned iblLightCount = 0;
    lights.count           = 0;
    for (auto& entity : a_Scene->GetVisibleEntities().lights) {
        const auto& lightData = entity.GetComponent<Component::LightData>();
        if (lightData.GetType() == LIGHT_TYPE_IBL && iblLightCount == VTFS_IBL_MAX)
            continue;
        lights.lights[lights.count] = ConvertLight(lightData, iblSamplers, iblLightCount);
        lights.count++;
        if (lights.count == VTFS_BUFFER_MAX)
            break;
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
