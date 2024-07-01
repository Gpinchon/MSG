#include <Renderer/OGL/CPULightCuller.hpp>
#include <Renderer/OGL/RAII/Buffer.hpp>
#include <Renderer/OGL/RAII/Wrapper.hpp>
#include <Renderer/OGL/Renderer.hpp>
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
struct CullingFunctor {
    CullingFunctor(
        const GLSL::mat4x4& a_MVP,
        const GLSL::VTFSLightsBuffer& a_Lights,
        GLSL::VTFSCluster* a_Clusters)
        : MVP(a_MVP)
        , lights(a_Lights)
        , clusters(a_Clusters)
    {
    }
    void operator()(const TabGraph::Tools::ComputeInputs& a_Input)
    {
        const auto clusterIndex = GLSL::VTFSClusterIndexTo1D(a_Input.workGroupID);
        auto& lightCluster      = clusters[clusterIndex];
        lightCluster.count      = 0;
        for (uint lightIndex = 0; lightIndex < lights.count; ++lightIndex) {
            const auto& light        = lights.lights[lightIndex];
            GLSL::vec3 lightPosition = light.position;
            float lightRadius        = light.range;
            GLSL::ProjectSphereToNDC(lightPosition, lightRadius, MVP);
            if (GLSL::SphereIntersectsAABB(
                    lightPosition, lightRadius,
                    lightCluster.aabb.minPoint, lightCluster.aabb.maxPoint)
                && lightCluster.count < VTFS_CLUSTER_MAX) {
                lightCluster.index[lightCluster.count] = lightIndex;
                lightCluster.count++;
            }
        }
    }
    const GLSL::mat4x4 MVP;
    const GLSL::VTFSLightsBuffer& lights;
    GLSL::VTFSCluster* clusters;
};

CPULightCuller::CPULightCuller(Renderer::Impl& a_Renderer)
    : _context(a_Renderer.context)
    , GPUlightsBuffer(RAII::MakePtr<RAII::Buffer>(_context, sizeof(_lights), &_lights, GL_DYNAMIC_STORAGE_BIT))
    , GPUclusters(RAII::MakePtr<RAII::Buffer>(_context, sizeof(_clusters), _clusters, GL_DYNAMIC_STORAGE_BIT))
{
    auto vtfsClusters = GLSL::GenerateVTFSClusters();
    for (uint i = 0; i < VTFS_CLUSTER_COUNT; ++i) {
        _clusters[i] = vtfsClusters[i];
    }
}

void TabGraph::Renderer::CPULightCuller::operator()(SG::Scene* a_Scene)
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
        GLSL::LightBase worldLight {
            SG::Node::GetWorldPosition(registry->GetEntityRef(entityID)),
            punctualLight.data.base.range
        };
        GLSL::vec3 lightPosition = worldLight.position;
        float lightRadius        = worldLight.range;
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
    CullingFunctor functor(MVP, _lights, _clusters);
    _compute.Dispatch(functor, { VTFS_CLUSTER_X, VTFS_CLUSTER_Y, VTFS_CLUSTER_Z });
    _context.PushCmd([this] {
        _compute.Wait();
        glNamedBufferSubData(
            GPUlightsBuffer->handle,
            0, sizeof(_lights), &_lights);
        glNamedBufferSubData(
            GPUclusters->handle,
            0, sizeof(_clusters), _clusters);
    });
}

}
