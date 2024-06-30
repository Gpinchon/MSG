#include <Renderer/OGL/CPULightCuller.hpp>
#include <Renderer/OGL/RAII/Wrapper.hpp>
#include <Renderer/OGL/Unix/Context.hpp>

#include <ECS/Registry.hpp>

#include <SG/Component/Camera.hpp>
#include <SG/Component/Light/PunctualLight.hpp>
#include <SG/Component/Transform.hpp>
#include <SG/Entity/Camera.hpp>
#include <SG/Scene/Scene.hpp>

#include <Lights.glsl>

#include <GL/glew.h>

#include <iostream>

namespace TabGraph::Renderer {
struct CullingFunctor {
    CullingFunctor(
        const GLSL::mat4x4& a_MVP,
        const GLSL::LightsBuffer& a_Lights,
        GLSL::LightCluster* a_Clusters)
        : MVP(a_MVP)
        , lights(a_Lights)
        , clusters(a_Clusters)
    {
    }
    void operator()(const TabGraph::Tools::ComputeInputs& a_Input)
    {
        const auto clusterIndex = GLSL::LightClusterIndexTo1D(a_Input.workGroupID);
        auto& lightCluster      = clusters[clusterIndex];
        lightCluster.count      = 0;
        for (uint lightIndex = 0; lightIndex < lights.count; ++lightIndex) {
            if (LightIntersectsAABB(CreateNDCLight(lights.lights[lightIndex], MVP), lightCluster.aabb) && lightCluster.count < LIGHT_CLUSTER_MAX) {
                lightCluster.index[lightCluster.count] = lightIndex;
                lightCluster.count++;
            }
        }
    }
    const GLSL::mat4x4 MVP;
    const GLSL::LightsBuffer& lights;
    GLSL::LightCluster* clusters;
};

/*
 * @brief transforms the NDC position to the Light clusters "position",
 * used to generate the Light clusters
 */
inline glm::vec3 LightClusterPosition(const glm::vec3& a_NDCPosition)
{
    return {
        a_NDCPosition.x, a_NDCPosition.y,
        pow(a_NDCPosition.z * 0.5f + 0.5f, LIGHT_CLUSTER_DEPTH_EXP) * 2.f - 1.f
    };
}

CPULightCuller::CPULightCuller(Context& a_Context)
    : _context(a_Context)
    , GPUlightsBuffer(RAII::MakePtr<RAII::Buffer>(_context, sizeof(_lights), &_lights, GL_DYNAMIC_STORAGE_BIT))
    , GPUclusters(RAII::MakePtr<RAII::Buffer>(_context, sizeof(_clusters), _clusters, GL_DYNAMIC_STORAGE_BIT))
{
    constexpr glm::vec3 clusterSize = {
        1.f / LIGHT_CLUSTER_X,
        1.f / LIGHT_CLUSTER_Y,
        1.f / LIGHT_CLUSTER_Z,
    };
    for (uint z = 0; z < LIGHT_CLUSTER_Z; ++z) {
        for (uint y = 0; y < LIGHT_CLUSTER_Y; ++y) {
            for (uint x = 0; x < LIGHT_CLUSTER_X; ++x) {
                glm::vec3 NDCMin           = (glm::vec3(x, y, z) * clusterSize) * 2.f - 1.f;
                glm::vec3 NDCMax           = NDCMin + clusterSize * 2.f;
                auto lightClusterIndex     = GLSL::LightClusterIndexTo1D({ x, y, z });
                auto& lightCluster         = _clusters[lightClusterIndex];
                lightCluster.aabb.minPoint = LightClusterPosition(NDCMin);
                lightCluster.aabb.maxPoint = LightClusterPosition(NDCMax);
            }
        }
    }
}

void TabGraph::Renderer::CPULightCuller::operator()(SG::Scene* a_Scene)
{
    _lights.count   = 0;
    auto registry   = a_Scene->GetRegistry();
    auto cameraView = SG::Camera::GetViewMatrix(a_Scene->GetCamera());
    auto cameraProj = a_Scene->GetCamera().GetComponent<SG::Component::Camera>().projection.GetMatrix();
    auto MVP        = cameraProj * cameraView;
    GLSL::LightClusterAABB cameraFrustum;
    cameraFrustum.minPoint = { -1, -1, -1 };
    cameraFrustum.maxPoint = { 1, 1, 1 };
    auto registryView      = registry->GetView<SG::Component::PunctualLight, SG::Component::Transform>();
    // pre-cull lights
    for (const auto& [entityID, punctualLight, transform] : registryView) {
        GLSL::LightBase worldLight {
            SG::Node::GetWorldPosition(registry->GetEntityRef(entityID)),
            punctualLight.data.base.range
        };
        if (LightIntersectsAABB(CreateNDCLight(worldLight, MVP), cameraFrustum)) {
            _lights.lights[_lights.count] = worldLight;
            _lights.count++;
            if (_lights.count == LIGHT_BUFFER_MAX)
                break;
        }
    }
    CullingFunctor functor(MVP, _lights, _clusters);
    //_compute.Wait();
    _compute.Dispatch(functor, { LIGHT_CLUSTER_X, LIGHT_CLUSTER_Y, LIGHT_CLUSTER_Z });
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
