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

#include <LightsVTFS.glsl>

#include <GL/glew.h>

MSG::Renderer::LightCullerVTFSBuffer::LightCullerVTFSBuffer(MSG::OGLContext& a_Ctx)
    : lightsBuffer(std::make_shared<OGLBuffer>(a_Ctx, sizeof(GLSL::VTFSLightsBuffer), nullptr, GL_DYNAMIC_STORAGE_BIT))
    , cluster(std::make_shared<OGLBuffer>(a_Ctx, sizeof(GLSL::VTFSCluster) * VTFS_CLUSTER_COUNT, GLSL::GenerateVTFSClusters().data(), GL_NONE))
{
}

MSG::Renderer::LightCullerVTFS::LightCullerVTFS(Renderer::Impl& a_Renderer)
    : _context(a_Renderer.context)
    , _cullingProgram(a_Renderer.shaderCompiler.CompileProgram("VTFSCulling"))
    , buffer(_buffers.front())
{
}

void MSG::Renderer::LightCullerVTFS::operator()(MSG::Scene* a_Scene, const std::shared_ptr<OGLBuffer>& a_CameraUBO)
{
    Prepare();
    const auto& registry = *a_Scene->GetRegistry();
    for (auto& entity : a_Scene->GetVisibleEntities().lights) {
        if (!PushLight(registry.GetComponent<Component::LightData>(entity)))
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
                         cullingProgram  = _cullingProgram,
                         GPUlightsBuffer = buffer.lightsBuffer,
                         GPUclusters     = buffer.cluster,
                         &CPULightBuffer = buffer.lightsBufferCPU] {
        auto lightBufferSize = offsetof(GLSL::VTFSLightsBuffer, lights) + (sizeof(GLSL::LightBase) * CPULightBuffer.count);
        // upload data
        glInvalidateBufferSubData(*GPUlightsBuffer, 0, lightBufferSize);
        glNamedBufferSubData(*GPUlightsBuffer, 0, lightBufferSize, &CPULightBuffer);
        // bind objects
        glBindBufferBase(GL_UNIFORM_BUFFER, UBO_CAMERA, *cameraUBO);
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
