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

/*
 * @brief VTFS clusters bounding box are generated only once and never change so they're only generated on the CPU
 */
INLINE std::vector<MSG::Renderer::GLSL::VTFSCluster> GenerateVTFSClusters()
{
    constexpr glm::vec3 clusterSize = {
        1.f / VTFS_CLUSTER_X,
        1.f / VTFS_CLUSTER_Y,
        1.f / VTFS_CLUSTER_Z,
    };
    std::vector<MSG::Renderer::GLSL::VTFSCluster> clusters(VTFS_CLUSTER_COUNT);
    for (size_t z = 0; z < VTFS_CLUSTER_Z; ++z) {
        for (size_t y = 0; y < VTFS_CLUSTER_Y; ++y) {
            for (size_t x = 0; x < VTFS_CLUSTER_X; ++x) {
                glm::vec3 NDCMin           = (glm::vec3(x, y, z) * clusterSize) * 2.f - 1.f;
                glm::vec3 NDCMax           = NDCMin + clusterSize * 2.f;
                auto lightClusterIndex     = MSG::Renderer::GLSL::VTFSClusterIndexTo1D({ x, y, z });
                auto& lightCluster         = clusters[lightClusterIndex];
                lightCluster.aabb.minPoint = MSG::Renderer::GLSL::VTFSClusterPosition(NDCMin);
                lightCluster.aabb.maxPoint = MSG::Renderer::GLSL::VTFSClusterPosition(NDCMax);
            }
        }
    }
    return clusters;
}

MSG::Renderer::LightCullerVTFSBuffer::LightCullerVTFSBuffer(MSG::OGLContext& a_Ctx)
    : lightsBuffer(std::make_shared<OGLTypedBuffer<GLSL::VTFSLightsBuffer>>(a_Ctx))
    , cluster(std::make_shared<OGLTypedBufferArray<GLSL::VTFSCluster>>(a_Ctx, VTFS_CLUSTER_COUNT, GenerateVTFSClusters().data()))
    , cmdBuffer(a_Ctx)
{
}

MSG::Renderer::LightCullerVTFS::LightCullerVTFS(Renderer::Impl& a_Renderer)
    : _context(a_Renderer.context)
    , _cullingProgram(a_Renderer.shaderCompiler.CompileProgram("VTFSCulling"))
    , buffer(&_buffers.front())
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
    buffer                        = &_buffers.at(_currentBuffer);
    buffer->lightsBufferCPU.count = 0;
}

void MSG::Renderer::LightCullerVTFS::Cull(const std::shared_ptr<OGLBuffer>& a_CameraUBO)
{
    buffer->lightsBuffer->Set(buffer->lightsBufferCPU);
    buffer->lightsBuffer->Update();
    OGLComputePipelineInfo cp;
    cp.bindings.uniformBuffers[UBO_CAMERA] = { .buffer = a_CameraUBO, .offset = 0, .size = a_CameraUBO->size };
    cp.bindings.storageBuffers[0]          = { .buffer = buffer->lightsBuffer, .offset = 0, .size = buffer->lightsBuffer->size };
    cp.bindings.storageBuffers[1]          = { .buffer = buffer->cluster, .offset = 0, .size = buffer->cluster->size };
    cp.shaderState.program                 = _cullingProgram;
    buffer->executionFence.Wait();
    buffer->executionFence.Reset();
    buffer->cmdBuffer.Reset();
    buffer->cmdBuffer.Begin();
    buffer->cmdBuffer.PushCmd<OGLCmdPushPipeline>(cp);
    buffer->cmdBuffer.PushCmd<OGLCmdDispatchCompute>(OGLCmdDispatchComputeInfo {
        .workgroupX = VTFS_CLUSTER_COUNT / VTFS_LOCAL_SIZE,
        .workgroupY = 1,
        .workgroupZ = 1,
    });
    buffer->cmdBuffer.End();
    buffer->cmdBuffer.Execute(&buffer->executionFence);
    _currentBuffer = (++_currentBuffer) % _buffers.size();
}
