#include <MSG/Camera.hpp>
#include <MSG/ECS/Registry.hpp>
#include <MSG/Entity/Camera.hpp>
#include <MSG/Light/PunctualLight.hpp>
#include <MSG/OGLBuffer.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLProgram.hpp>
#include <MSG/Renderer/OGL/Components/LightData.hpp>
#include <MSG/Renderer/OGL/LightCullerVTFS.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/ShaderCompiler.hpp>
#include <MSG/Scene.hpp>
#include <MSG/Transform.hpp>

#include <VTFS.glsl>

#include <GL/glew.h>

template <typename LightType>
inline bool MSG::Renderer::LightCullerVTFS::PushLight(const LightType& a_Light)
{
    auto& lights = buffer.lightsBufferCPU;
    if (lights.count >= VTFS_BUFFER_MAX) [[unlikely]]
        return false;
    lights.lights[lights.count] = *reinterpret_cast<const GLSL::LightBase*>(&a_Light);
    lights.count++;
    return true;
}

template <>
inline bool MSG::Renderer::LightCullerVTFS::PushLight(const Component::LightData& a_LightData)
{
    return std::visit([this](auto& a_Data) mutable { return PushLight(a_Data); }, a_LightData);
}

MSG::Renderer::LightCullerVTFSBuffer::LightCullerVTFSBuffer(MSG::OGLContext& a_Ctx)
    : lightsBuffer(std::make_shared<OGLBuffer>(a_Ctx, sizeof(GLSL::VTFSLightsBuffer), nullptr, GL_DYNAMIC_STORAGE_BIT))
    , cluster(std::make_shared<OGLBuffer>(a_Ctx, sizeof(GLSL::VTFSCluster) * VTFS_CLUSTER_COUNT, GLSL::GenerateVTFSClusters().data(), GL_NONE))
{
}

MSG::Renderer::LightCullerVTFS::LightCullerVTFS(Renderer::Impl& a_Renderer)
    : _context(a_Renderer.context)
    , _vtfsCullingProgram(a_Renderer.shaderCompiler.CompileProgram("VTFSCulling"))
    , buffer(_buffers.front())
{
}

void MSG::Renderer::LightCullerVTFS::operator()(MSG::Scene* a_Scene, const std::shared_ptr<OGLBuffer>& a_CameraUBO)
{
    Prepare();
    for (auto& entity : a_Scene->GetVisibleEntities().lights) {
        if (!PushLight(entity.GetComponent<Component::LightData>()))
            break;
    }

    Cull(a_CameraUBO);
}

void MSG::Renderer::LightCullerVTFS::Prepare()
{
    buffer = _buffers.at(_currentBuffer);
    buffer.clear();
}

void MSG::Renderer::LightCullerVTFS::Cull(const std::shared_ptr<OGLBuffer>& a_CameraUBO)
{
    _context.PushCmd([cameraUBO          = a_CameraUBO,
                         cullingProgram  = _vtfsCullingProgram,
                         GPUlightsBuffer = buffer.lightsBuffer,
                         GPUclusters     = buffer.cluster,
                         &CPULightBuffer = buffer.lightsBufferCPU] {
        auto lightBufferSize = offsetof(GLSL::VTFSLightsBuffer, lights) + (sizeof(GLSL::LightBase) * CPULightBuffer.count);
        // upload data
        glInvalidateBufferSubData(*GPUlightsBuffer, 0, lightBufferSize);
        glNamedBufferSubData(*GPUlightsBuffer, 0, lightBufferSize, &CPULightBuffer);
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
    _currentBuffer = (++_currentBuffer) % _buffers.size();
}
