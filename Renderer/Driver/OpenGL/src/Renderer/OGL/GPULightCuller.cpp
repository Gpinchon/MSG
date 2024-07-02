#include <Renderer/OGL/GPULightCuller.hpp>
#include <Renderer/OGL/RAII/Buffer.hpp>
#include <Renderer/OGL/RAII/Program.hpp>
#include <Renderer/OGL/RAII/Wrapper.hpp>
#include <Renderer/OGL/Renderer.hpp>
#include <Renderer/OGL/ShaderCompiler.hpp>
#include <Renderer/OGL/Unix/Context.hpp>

#include <ECS/Registry.hpp>

#include <SG/Component/Camera.hpp>
#include <SG/Component/Light/PunctualLight.hpp>
#include <SG/Component/Transform.hpp>
#include <SG/Entity/Camera.hpp>
#include <SG/Scene/Scene.hpp>

#include <VTFS.glsl>

#include <GL/glew.h>

namespace TabGraph::Renderer {

GPULightCuller::GPULightCuller(Renderer::Impl& a_Renderer)
    : _renderer(a_Renderer)
    , _cullingProgram(a_Renderer.shaderCompiler.CompileProgram("VTFSCulling"))
    , GPUclusters(RAII::MakePtr<RAII::Buffer>(_renderer.context, sizeof(GLSL::VTFSCluster) * VTFS_CLUSTER_COUNT, GLSL::GenerateVTFSClusters().data(), 0))
{
    for (auto& buffer : _GPUlightsBuffers) {
        buffer = RAII::MakePtr<RAII::Buffer>(_renderer.context, sizeof(GLSL::VTFSLightsBuffer), nullptr, GL_DYNAMIC_STORAGE_BIT);
    }
    GPUlightsBuffer = _GPUlightsBuffers.at(0);
}

void GPULightCuller::operator()(SG::Scene* a_Scene)
{
    _lights.count   = 0;
    auto registry   = a_Scene->GetRegistry();
    auto cameraView = SG::Camera::GetViewMatrix(a_Scene->GetCamera());
    auto cameraProj = a_Scene->GetCamera().GetComponent<SG::Component::Camera>().projection.GetMatrix();
    auto MVP        = cameraProj * cameraView;
    GLSL::VTFSClusterAABB cameraFrustum;
    cameraFrustum.minPoint = { -1, -1, -1 };
    cameraFrustum.maxPoint = { 1, 1, 1 };
    auto registryView      = registry->GetView<SG::Component::PunctualLight, SG::Component::Transform>();
    // pre-cull lights
    for (const auto& [entityID, punctualLight, transform] : registryView) {
        GLSL::LightBase worldLight;
        worldLight.commonData.type      = int(punctualLight.type);
        worldLight.commonData.position  = SG::Node::GetWorldPosition(registry->GetEntityRef(entityID));
        worldLight.commonData.color     = punctualLight.data.base.color;
        worldLight.commonData.range     = punctualLight.data.base.range;
        worldLight.commonData.intensity = punctualLight.data.base.intensity;
        worldLight.commonData.falloff   = punctualLight.data.base.falloff;
        GLSL::vec3 lightPosition        = worldLight.commonData.position;
        float lightRadius               = worldLight.commonData.range;
        GLSL::ProjectSphereToNDC(lightPosition, lightRadius, MVP);
        if (GLSL::SphereIntersectsAABB(
                lightPosition, lightRadius,
                cameraFrustum.minPoint, cameraFrustum.maxPoint)) {
            _lights.lights[_lights.count] = worldLight;
            _lights.count++;
            if (_lights.count == VTFS_BUFFER_MAX)
                break;
        }
    }
    _renderer.context.PushCmd([this] {
        glNamedBufferSubData(
            *GPUlightsBuffer,
            0, sizeof(_lights), &_lights);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, *_renderer.forwardCameraUBO.buffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, *GPUlightsBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, *GPUclusters);
        glUseProgram(*_cullingProgram);
        glDispatchCompute(VTFS_CLUSTER_COUNT / VTFS_LOCAL_SIZE, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        //  unbind objects
        glUseProgram(0);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
    });
    GPUlightsBuffer     = _GPUlightsBuffers.at(_currentLightBuffer);
    _currentLightBuffer = (++_currentLightBuffer) % _GPUlightsBuffers.size();
}

}
