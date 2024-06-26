#include <Renderer/OGL/LightCuller.hpp>

#include <ECS/Registry.hpp>

#include <SG/Component/Camera.hpp>
#include <SG/Component/Light/PunctualLight.hpp>
#include <SG/Component/Transform.hpp>
#include <SG/Entity/Camera.hpp>
#include <SG/Scene/Scene.hpp>

#include <Lights.glsl>

#include <iostream>

namespace TabGraph::Renderer {
bool LightIntersectsAABB(const GLSL::LightBase& a_Light, const GLSL::LightClusterAABB& a_AABB)
{
    // closest point on the AABB to the sphere center
    auto closestPoint = glm::clamp(a_Light.position, a_AABB.minPoint, a_AABB.maxPoint);
    auto diff         = closestPoint - a_Light.position;
    // squared distance between the sphere center and closest point
    float distanceSquared = glm::dot(diff, diff);
    return distanceSquared <= a_Light.range * a_Light.range;
    // double dmin = 0;
    // for (int i = 0; i < decltype(a_Light.position)::length(); i++) {
    //     if (a_Light.position[i] < a_AABB.minPoint[i])
    //         dmin += pow(a_Light.position[i] - a_AABB.minPoint[i], 2);
    //     else if (a_Light.position[i] > a_AABB.maxPoint[i])
    //         dmin += pow(a_Light.position[i] - a_AABB.maxPoint[i], 2);
    // }
    // return dmin <= pow(a_Light.range, 2);
}

GLSL::LightBase CreateNDCLight(const GLSL::LightBase& a_WorldLight, const glm::mat4& a_MVP)
{
    auto worldLightLimit = glm::vec3(
        a_WorldLight.position.x + a_WorldLight.range,
        a_WorldLight.position.y,
        a_WorldLight.position.z);
    auto viewLightPos   = a_MVP * glm::vec4(a_WorldLight.position, 1);
    auto viewLightLimit = a_MVP * glm::vec4(worldLightLimit, 1);
    auto NDCLightPos    = glm::vec3(viewLightPos) / viewLightPos.w;
    auto NDCLightLimit  = glm::vec3(viewLightLimit) / viewLightLimit.w;
    auto NDCLightRange  = glm::distance(NDCLightPos, NDCLightLimit);
    return { NDCLightPos, NDCLightRange };
}

LightCuller::LightCuller()
{
    constexpr glm::vec3 NDCFrustumMin = { -1, -1, -1 };
    float NDCFrustumDistance          = 2;
    glm::vec3 clusterSize             = {
        NDCFrustumDistance / LIGHT_CLUSTER_X,
        NDCFrustumDistance / LIGHT_CLUSTER_Y,
        NDCFrustumDistance / LIGHT_CLUSTER_Z,
    };
    glm::vec3 clusterMin;
    glm::vec3 clusterMax;
    for (uint x = 0; x < LIGHT_CLUSTER_X; ++x) {
        clusterMin.x = NDCFrustumMin.x + x * clusterSize.x;
        clusterMax.x = clusterMin.x + clusterSize.x;
        for (uint y = 0; y < LIGHT_CLUSTER_Y; ++y) {
            clusterMin.y = NDCFrustumMin.y + y * clusterSize.y;
            clusterMax.y = clusterMin.y + clusterSize.y;
            for (uint z = 0; z < LIGHT_CLUSTER_Z; ++z) {
                clusterMin.z                            = NDCFrustumMin.z + z * clusterSize.z;
                clusterMax.z                            = clusterMin.z + clusterSize.z;
                auto clusterIndex                       = GLSL::LightClusterTo1D(x, y, z);
                clusters.at(clusterIndex).aabb.minPoint = clusterMin;
                clusters.at(clusterIndex).aabb.maxPoint = clusterMax;
            }
        }
    }
}

struct CullingFunctor {
    CullingFunctor(
        const GLSL::mat4x4& a_MVP,
        const uint& a_LightCount, const std::array<GLSL::LightBase, 1024>& a_Lights,
        std::array<GLSL::LightCluster, LIGHT_CLUSTER_COUNT>& a_Clusters)
        : MVP(a_MVP)
        , lightCount(a_LightCount)
        , lights(a_Lights)
        , clusters(a_Clusters)
    {
    }
    void operator()(const TabGraph::Tools::ComputeInputs& a_Input)
    {
        const auto clusterIndex = GLSL::LightClusterTo1D(a_Input.workGroupID.x, a_Input.workGroupID.y, a_Input.workGroupID.z);
        auto& lightCluster      = clusters[clusterIndex];
        lightCluster.count      = 0;
        for (uint lightIndex = 0; lightIndex < lightCount; ++lightIndex) {
            auto NDCLight = CreateNDCLight(lights[lightIndex], MVP);
            if (LightIntersectsAABB(NDCLight, lightCluster.aabb) && lightCluster.count < LIGHT_CLUSTER_MAX) {
                lightCluster.index[lightCluster.count] = lightIndex;
                lightCluster.count++;
            }
        }
    }
    const GLSL::mat4x4 MVP;
    const uint& lightCount;
    const std::array<GLSL::LightBase, 1024>& lights;
    std::array<GLSL::LightCluster, LIGHT_CLUSTER_COUNT>& clusters;
};

void TabGraph::Renderer::LightCuller::operator()(SG::Scene* a_Scene)
{
    lightCount      = 0;
    auto registry   = a_Scene->GetRegistry();
    auto cameraView = SG::Camera::GetViewMatrix(a_Scene->GetCamera());
    auto cameraProj = a_Scene->GetCamera().GetComponent<SG::Component::Camera>().projection.GetMatrix();
    auto MVP        = cameraProj * cameraView;
    GLSL::LightClusterAABB cameraFrustum {
        { -1, -1, -1 },
        { 1, 1, 1 }
    };
    // pre-cull lights
    for (const auto& [entityID, punctualLight, transform] : a_Scene->GetRegistry()->GetView<SG::Component::PunctualLight, SG::Component::Transform>()) {
        GLSL::LightBase worldLight {
            SG::Node::GetWorldPosition(registry->GetEntityRef(entityID)),
            punctualLight.data.base.range
        };
        if (LightIntersectsAABB(CreateNDCLight(worldLight, MVP), cameraFrustum)) {
            lights.at(lightCount) = worldLight;
            lightCount++;
            if (lightCount == lights.size())
                break;
        }
    }
    CullingFunctor functor(MVP, lightCount, lights, clusters);
    _compute.Dispatch(functor, { LIGHT_CLUSTER_X, LIGHT_CLUSTER_Y, LIGHT_CLUSTER_Z });
    _compute.Wait();
}

}
