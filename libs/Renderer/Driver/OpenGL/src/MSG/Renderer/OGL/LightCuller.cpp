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
static GLSL::LightBase ConvertLight(const LightType& a_Light)
{
    return *reinterpret_cast<const GLSL::LightBase*>(&a_Light);
}

static GLSL::LightBase ConvertLight(const Component::LightData& a_LightData)
{
    return std::visit([](auto& a_Data) mutable {
        return ConvertLight(a_Data);
    },
        a_LightData);
}

GPULightCullerVTFSBuffers::GPULightCullerVTFSBuffers(Renderer::Impl& a_Renderer)
    : lightsBuffer(std::make_shared<OGLBuffer>(a_Renderer.context, sizeof(GLSL::VTFSLightsBuffer), nullptr, GL_DYNAMIC_STORAGE_BIT))
    , cluster(std::make_shared<OGLBuffer>(a_Renderer.context, sizeof(GLSL::VTFSCluster) * VTFS_CLUSTER_COUNT, GLSL::GenerateVTFSClusters().data(), GL_NONE))
{
}

GPULightCuller::GPULightCuller(Renderer::Impl& a_Renderer)
    : _renderer(a_Renderer)
    , _vtfsCullingProgram(a_Renderer.shaderCompiler.CompileProgram("VTFSCulling"))
    , vtfs(_vtfs.front())
    , ibl(_ibl.front())
{
}

void GPULightCuller::operator()(Scene* a_Scene)
{
    vtfs                   = _vtfs.at(_currentLightBuffer);
    ibl                    = _ibl.at(_currentLightBuffer);
    auto& lights           = vtfs.lightsBufferCPU;
    unsigned iblLightCount = 0;
    lights.count           = 0;
    ibl.clear();
    for (auto& entity : a_Scene->GetVisibleEntities().lights) {
        const auto& lightData = entity.GetComponent<Component::LightData>();
        if (lightData.GetType() == LIGHT_TYPE_IBL) [[unlikely]] {
            auto& iblLight          = std::get<Component::LightIBLData>(lightData);
            auto& glslLight         = ibl.lights.emplace_back();
            glslLight.commonData    = iblLight.commonData;
            glslLight.halfSize      = iblLight.halfSize;
            glslLight.specularIndex = ibl.samplers.size();
            for (auto i = 0; i < iblLight.irradianceCoefficients.size(); i++)
                glslLight.irradianceCoefficients[i] = glm::vec4(iblLight.irradianceCoefficients[i], 0);
            ibl.samplers.emplace_back(iblLight.specular);
        } else if (lights.count < VTFS_BUFFER_MAX) [[likely]] {
            lights.lights[lights.count] = ConvertLight(lightData);
            lights.count++;
        }
    }
    _renderer.context.PushCmd([cameraUBO          = _renderer.cameraUBO.buffer,
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
