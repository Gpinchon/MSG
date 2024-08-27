#include <Renderer/OGL/CPULightCuller.hpp>
#include <Renderer/OGL/RAII/Buffer.hpp>
#include <Renderer/OGL/RAII/Wrapper.hpp>
#include <Renderer/OGL/Renderer.hpp>

#ifdef WIN32
#include <Renderer/OGL/Win32/Context.hpp>
#elif defined __linux__
#include <Renderer/OGL/Unix/Context.hpp>
#endif // WIN32

#include <ECS/Registry.hpp>

#include <SG/Component/Camera.hpp>
#include <SG/Component/Light/PunctualLight.hpp>
#include <SG/Component/Transform.hpp>
#include <SG/Entity/Camera.hpp>
#include <SG/Scene/Scene.hpp>

#include <VTFS.glsl>

#include <GL/glew.h>

namespace TabGraph::Renderer {
GLSL::LightBase GetGLSLLight(
    const SG::Component::PunctualLight& a_SGLight,
    const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    GLSL::LightBase glslLight;
    glslLight.commonData.position = SG::Node::GetWorldPosition(a_Entity);
    std::visit([&glslLight](auto& a_Data) {
        glslLight.commonData.intensity = a_Data.intensity;
        glslLight.commonData.color     = a_Data.color;
        glslLight.commonData.falloff   = a_Data.falloff;
    },
        a_SGLight);
    switch (a_SGLight.GetType()) {
    case SG::Component::LightType::Point: {
        glslLight.commonData.type = LIGHT_TYPE_POINT;
        auto& point               = reinterpret_cast<GLSL::LightPoint&>(glslLight);
        point.range               = std::get<SG::Component::LightPoint>(a_SGLight).range;
    } break;
    case SG::Component::LightType::Spot: {
        glslLight.commonData.type = LIGHT_TYPE_SPOT;
        auto& spot                = reinterpret_cast<GLSL::LightSpot&>(glslLight);
        spot.direction            = SG::Node::GetForward(a_Entity);
        spot.range                = std::get<SG::Component::LightSpot>(a_SGLight).range;
        spot.innerConeAngle       = std::get<SG::Component::LightSpot>(a_SGLight).innerConeAngle;
        spot.outerConeAngle       = std::get<SG::Component::LightSpot>(a_SGLight).outerConeAngle;
    } break;
    case SG::Component::LightType::Directional: {
        glslLight.commonData.type = LIGHT_TYPE_DIRECTIONAL;
        auto& dir                 = reinterpret_cast<GLSL::LightDirectional&>(glslLight);
        dir.halfSize              = std::get<SG::Component::LightDirectional>(a_SGLight).halfSize;
    } break;
    case SG::Component::LightType::IBL: {
        glslLight.commonData.type = LIGHT_TYPE_DIRECTIONAL;
        auto& dir                 = reinterpret_cast<GLSL::LightIBL&>(glslLight);
        dir.halfSize              = std::get<SG::Component::LightIBL>(a_SGLight).halfSize;
    } break;
    default:
        break;
    }
    return glslLight;
}

bool LightIntersects(
    IN(GLSL::LightBase) a_Light,
    IN(glm::mat4x4) a_MVP,
    IN(glm::vec3) a_AABBMin,
    IN(glm::vec3) a_AABBMax)
{
    if (a_Light.commonData.type == LIGHT_TYPE_POINT) {
        glm::vec3 lightPosition = a_Light.commonData.position;
        float lightRadius       = reinterpret_cast<const GLSL::LightPoint&>(a_Light).range;
        GLSL::ProjectSphereToNDC(lightPosition, lightRadius, a_MVP);
        return GLSL::SphereIntersectsAABB(
            lightPosition, lightRadius,
            a_AABBMin, a_AABBMax);
    } else if (a_Light.commonData.type == LIGHT_TYPE_SPOT) {
        auto& spot               = reinterpret_cast<const GLSL::LightSpot&>(a_Light);
        glm::vec3 lightPosition  = spot.commonData.position;
        glm::vec3 lightDirection = spot.direction;
        float lightRadius        = spot.range;
        float lightAngle         = spot.outerConeAngle;
        GLSL::ProjectConeToNDC(lightPosition, lightDirection, lightRadius, a_MVP);
        return GLSL::ConeIntersectsAABB(
            lightPosition, lightDirection, lightAngle, lightRadius,
            a_AABBMin, a_AABBMax);
    } else if (a_Light.commonData.type == LIGHT_TYPE_IBL) {
        auto& ibl          = reinterpret_cast<const GLSL::LightIBL&>(a_Light);
        auto lightRadius   = length(ibl.halfSize);
        auto lightPosition = ibl.commonData.position;
        GLSL::ProjectSphereToNDC(lightPosition, lightRadius, a_MVP);
        return GLSL::SphereIntersectsAABB(
            lightPosition, lightRadius,
            a_AABBMin, a_AABBMax);
    }
    return false;
}

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
        for (uint32_t lightIndex = 0; lightIndex < lights.count; ++lightIndex) {
            const auto& light = lights.lights[lightIndex];
            if (LightIntersects(
                    light, MVP,
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
    for (uint32_t i = 0; i < VTFS_CLUSTER_COUNT; ++i) {
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
        auto entity     = registry->GetEntityRef(entityID);
        auto worldLight = GetGLSLLight(punctualLight, entity);
        if (LightIntersects(worldLight, MVP, cameraFrustum.minPoint, cameraFrustum.maxPoint)) {
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
