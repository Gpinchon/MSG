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
        const auto clusterIndex = a_Input.workGroupSize.x * a_Input.workGroupID.x + a_Input.localInvocationID.x;
        auto& lightCluster      = clusters[clusterIndex];
        lightCluster.count      = 0;
        for (uint lightIndex = 0; lightIndex < lights.count; ++lightIndex) {
            const auto& light        = lights.lights[lightIndex];
            GLSL::vec3 lightPosition = light.commonData.position;
            float lightRadius        = light.commonData.range;
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

void GetGLSLLight(
    GLSL::LightBase& a_GLSLLight,
    const SG::Component::PunctualLight& a_SGLight)
{
    switch (a_SGLight.GetType()) {
    case SG::Component::LightType::Point:
        a_GLSLLight.commonData.type = LIGHT_TYPE_POINT;
        break;
    case SG::Component::LightType::Directional: {
        a_GLSLLight.commonData.type = LIGHT_TYPE_DIRECTIONAL;
        auto& dirLight              = reinterpret_cast<GLSL::LightDirectional&>(a_GLSLLight);
        dirLight.halfSize           = std::get<SG::Component::LightDirectional>(a_SGLight).halfSize;
    } break;
    case SG::Component::LightType::Spot: {
        a_GLSLLight.commonData.type = LIGHT_TYPE_SPOT;
        auto& spot                  = reinterpret_cast<GLSL::LightSpot&>(a_GLSLLight);
        spot.innerConeAngle         = std::get<SG::Component::LightSpot>(a_SGLight).innerConeAngle;
        spot.outerConeAngle         = std::get<SG::Component::LightSpot>(a_SGLight).outerConeAngle;
    } break;
    default:
        break;
    }
}

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
        GLSL::LightBase worldLight;
        worldLight.commonData.position = SG::Node::GetWorldPosition(registry->GetEntityRef(entityID));
        std::visit([&worldLight](auto& a_Data) {
            worldLight.commonData.intensity = a_Data.intensity;
            worldLight.commonData.range     = a_Data.range;
            worldLight.commonData.color     = a_Data.color;
            worldLight.commonData.falloff   = a_Data.falloff;
        },
            punctualLight);
        GLSL::vec3 lightPosition = worldLight.commonData.position;
        float lightRadius        = worldLight.commonData.range;
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
    _compute.Dispatch(functor, { VTFS_CLUSTER_COUNT / VTFS_LOCAL_SIZE, 1, 1 });
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
