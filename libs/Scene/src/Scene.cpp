/*
 * @Author: gpinchon
 * @Date:   2021-06-19 15:04:37
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-06-19 15:04:37
 */

#include <Core/Camera.hpp>
#include <Core/Children.hpp>
#include <Core/Light/PunctualLight.hpp>
#include <Core/Mesh.hpp>
#include <Core/MeshSkin.hpp>
#include <Core/Transform.hpp>
#include <Scene.hpp>
#include <Tools/Debug.hpp>

#include <format>
#include <ranges>

namespace MSG {
Scene::Scene()
    : Inherit()
{
    static auto s_sceneNbr { 0u };
    s_sceneNbr++;
    SetName(std::format("Scene_{}", s_sceneNbr));
}

template <bool Root, typename EntityRefType>
static Core::BoundingVolume& UpdateBoundingVolume(
    EntityRefType& a_Entity,
    const Core::BoundingVolume& a_BaseBV,
    std::vector<Core::BoundingVolume*>& a_InfiniteBV)
{
    auto& bv           = a_Entity.template GetComponent<Core::BoundingVolume>();
    auto& transform    = a_Entity.template GetComponent<Core::Transform>();
    auto& transformMat = transform.GetWorldTransformMatrix();
    auto hasLight      = a_Entity.template HasComponent<Core::PunctualLight>();
    auto hasMesh       = a_Entity.template HasComponent<Core::Mesh>();
    auto hasMeshSkin   = a_Entity.template HasComponent<Core::MeshSkin>();
    auto hasChildren   = a_Entity.template HasComponent<Core::Children>();
    bv                 = { transform.GetWorldPosition(), { 0, 0, 0 } };
    if (hasMeshSkin) {
        auto& skin = a_Entity.template GetComponent<Core::MeshSkin>();
        bv += skin.ComputeBoundingVolume();
    } else if (hasMesh) {
        auto& mesh = a_Entity.template GetComponent<Core::Mesh>();
        bv += transformMat * mesh.geometryTransform * mesh.boundingVolume;
    }
    if (hasLight) {
        auto& light        = a_Entity.template GetComponent<Core::PunctualLight>();
        auto lightHalfSize = light.GetHalfSize();
        bv += Core::BoundingVolume(transform.GetWorldPosition(), lightHalfSize);
    }
    if (hasChildren) {
        for (auto& child : a_Entity.template GetComponent<Core::Children>()) {
            auto& childBV = UpdateBoundingVolume<false>(child, bv, a_InfiniteBV);
            if (childBV.IsInf()) {
                a_InfiniteBV.emplace_back(&childBV);
                if constexpr (!Root)
                    bv += childBV;
            } else
                bv += childBV;
        }
    }
    return bv;
}

void Scene::UpdateBoundingVolumes()
{
    std::vector<Core::BoundingVolume*> infiniteBV;
    auto& newBV = UpdateBoundingVolume<true>(GetRootEntity(), GetBoundingVolume(), infiniteBV);
    SetBoundingVolume(newBV);
    for (auto& bv : infiniteBV)
        *bv = GetBoundingVolume(); // set the infinite BV to the scene's BV
}

template <typename EntityRefType, typename OctreeType, typename OctreeRefType>
void InsertEntity(EntityRefType& a_Entity, OctreeType& a_Octree, const OctreeRefType& a_Ref)
{
    auto& bv = a_Entity.template GetComponent<Core::BoundingVolume>();
    auto ref = a_Octree.Insert(a_Ref, a_Entity, bv);
    if (!ref.first)
        return;
    if (a_Entity.template HasComponent<Core::Children>()) {
        for (auto& child : a_Entity.template GetComponent<Core::Children>()) {
            InsertEntity(child, a_Octree, ref.second);
        }
    }
}

Core::Transform& Scene::GetRootTransform()
{
    return GetRootEntity().GetComponent<Core::Transform>();
}

Core::Children& Scene::GetRootChildren()
{
    return GetRootEntity().GetComponent<Core::Children>();
}

void Scene::UpdateOctree()
{
    auto const& bv = GetBoundingVolume();
    // clear up octree
    GetOctree().Clear();
    GetOctree().SetMinMax(bv.Min() - 0.1f, bv.Max() + 0.1f);
    InsertEntity(GetRootEntity(), GetOctree(), OctreeType::RefType {});
}

template <typename EntityRefType>
auto ComputeLod(const EntityRefType& a_Entity, const glm::mat4x4& a_CameraVP, const float& a_LodBias)
{
    const auto& mesh           = a_Entity.template GetComponent<Core::Mesh>();
    const auto& bv             = a_Entity.template GetComponent<Core::BoundingVolume>();
    const auto viewBV          = a_CameraVP * bv;
    const auto viewSphere      = (Core::Sphere)viewBV;
    const float screenCoverage = std::min(viewSphere.radius, 1.f);
    uint8_t levelI             = 0;
    while (levelI < mesh.size()) {
        auto& level    = mesh.at(levelI);
        float coverage = level.screenCoverage + a_LodBias;
        if (screenCoverage >= coverage || levelI == (mesh.size() - 1))
            break;
        levelI++;
    }
    return levelI;
}

static glm::vec3 GetBVClosestPoint(const Core::BoundingVolume& a_BV, const Core::Plane& a_Plane)
{
    auto nx                         = a_Plane.GetNormal().x > 0 ? 1 : 0;
    auto ny                         = a_Plane.GetNormal().y > 0 ? 1 : 0;
    auto nz                         = a_Plane.GetNormal().z > 0 ? 1 : 0;
    std::array<glm::vec3, 2> minMax = { a_BV.Min(), a_BV.Max() };
    return { minMax[nx].x, minMax[ny].y, minMax[nz].z };
}

static bool BVInsideFrustum(const Core::BoundingVolume& a_BV, const Core::Frustum& a_Frustum)
{
    for (auto& plane : a_Frustum) {
        auto vn = GetBVClosestPoint(a_BV, plane);
        if (plane.GetDistance(vn) < 0) {
            return false;
        }
    }
    return true;
}

void Scene::CullEntities(const SceneCullSettings& a_CullSettings)
{
    if (GetCamera().Empty()) {
        errorLog("Scene has no camera, cannot cull entities.");
        return;
    }
    auto const& camera          = GetCamera().GetComponent<Core::Camera>();
    auto const& cameraTransform = GetCamera().GetComponent<Core::Transform>();
    auto frustum                = camera.projection.GetFrustum(cameraTransform);
    CullEntities(frustum, a_CullSettings, GetVisibleEntities());
}

void Scene::CullEntities(const Core::Frustum& a_Frustum, const SceneCullSettings& a_CullSettings, SceneCullResult& a_CullResult) const
{
    a_CullResult.Clear();
    auto const& camera           = GetCamera().GetComponent<Core::Camera>();
    auto const& cameraTransform  = GetCamera().GetComponent<Core::Transform>();
    auto const& cameraProjection = camera.projection.GetMatrix();
    auto const cameraView        = glm::inverse(cameraTransform.GetWorldTransformMatrix());
    auto const cameraVP          = cameraProjection * cameraView;

    auto hasPunctualLight = [](auto& a_Entity) { return a_Entity.template HasComponent<Core::PunctualLight>(); };
    auto hasMesh          = [](auto& a_Entity) { return a_Entity.template HasComponent<Core::Mesh>(); };
    auto hasMeshSkin      = [](auto& a_Entity) { return a_Entity.template HasComponent<Core::MeshSkin>(); };
    auto castsShadow      = [](auto& a_Entity) { return std::visit([](const auto& lightData) { return lightData.shadowSettings.castShadow; }, a_Entity.template GetComponent<Core::PunctualLight>()); };
    auto sortByDistance   = [&nearPlane = a_Frustum[Core::FrustumFace::Near]](auto& a_Lhs, auto& a_Rhs) {
        auto& lBv = a_Lhs.template GetComponent<Core::BoundingVolume>();
        auto& rBv = a_Rhs.template GetComponent<Core::BoundingVolume>();
        auto lCp  = GetBVClosestPoint(lBv, nearPlane);
        auto rCp  = GetBVClosestPoint(rBv, nearPlane);
        auto lDi  = std::abs(nearPlane.GetDistance(lCp));
        auto rDi  = std::abs(nearPlane.GetDistance(rCp));
        return lDi < rDi;
    };
    auto sortByPriority = [&sortByDistance](auto& a_Lhs, auto& a_Rhs) {
        auto& lPl = a_Lhs.template GetComponent<Core::PunctualLight>();
        auto& rPl = a_Rhs.template GetComponent<Core::PunctualLight>();
        auto lPr  = std::visit([](const auto& light) { return light.priority; }, lPl);
        auto rPr  = std::visit([](const auto& light) { return light.priority; }, rPl);
        if (lPr == rPr) // if priorities are equal, sort by distance
            return sortByDistance(a_Lhs, a_Rhs);
        return lPr > rPr;
    };
    auto cullVisitor = [this, &a_CullResult, &a_Frustum, &a_CullSettings, &cameraVP, hasMesh, hasPunctualLight](auto& node) mutable {
        if (node.Empty() || !BVInsideFrustum(node.Bounds(), a_Frustum))
            return false; // no other entities further down or we're outside frustum
        for (auto& entity : node.Storage()) {
            auto& bv = entity.template GetComponent<Core::BoundingVolume>();
            if (!BVInsideFrustum(bv, a_Frustum))
                continue;
            a_CullResult.entities.emplace_back(entity);
            if (a_CullSettings.cullMeshes && hasMesh(entity))
                a_CullResult.meshes.emplace_back(entity, ComputeLod(entity, cameraVP, GetLevelOfDetailsBias()));
            if (a_CullSettings.cullLights && hasPunctualLight(entity))
                a_CullResult.lights.emplace_back(entity);
        }
        return true;
    };
    a_CullResult.Reserve(GetRegistry()->Count());
    GetOctree().Visit(cullVisitor);

    std::ranges::sort(a_CullResult.meshes, sortByDistance);
    if (a_CullSettings.cullMeshSkins)
        std::ranges::copy_if(a_CullResult.meshes, std::back_inserter(a_CullResult.skins), hasMeshSkin); // No need to sort again

    std::ranges::sort(a_CullResult.lights, sortByPriority);
    if (a_CullSettings.cullShadows) {
        for (auto& light : a_CullResult.lights) {
            if (!castsShadow(light))
                continue;
            auto& shadowCaster     = a_CullResult.shadows.emplace_back(light);
            const auto& lightData  = shadowCaster.GetComponent<Core::PunctualLight>();
            const auto& castShadow = std::visit([](const auto& lightData) { return lightData.shadowSettings.castShadow; }, lightData);
            ;
            const auto& lightTransform = shadowCaster.GetComponent<Core::Transform>();
            auto lightView             = glm::inverse(lightTransform.GetWorldTransformMatrix());
            SceneCullResult shadowCullResult;
            SceneCullSettings shadowCullSettings {
                .cullMeshSkins = false,
                .cullLights    = false,
                .cullShadows   = false
            };
            if (lightData.GetType() == Core::LightType::Directional) {
                const auto& lightBV     = shadowCaster.GetComponent<Core::BoundingVolume>();
                const auto& lightBVProj = lightView * lightBV;
                Core::Projection::Orthographic orthoProj {
                    .xmag  = lightBVProj.halfSize.x,
                    .ymag  = lightBVProj.halfSize.y,
                    .znear = lightBVProj.Min().z,
                    .zfar  = lightBVProj.Max().z
                };
                auto lightProj    = Core::Projection(orthoProj);
                auto lightFrustum = lightProj.GetFrustum(lightTransform);
                CullEntities(lightFrustum, shadowCullSettings, shadowCullResult);
                auto& shadowViewport            = shadowCaster.shadowViewports.emplace_back();
                shadowViewport.projectionMatrix = lightProj * lightView;
                shadowViewport.meshes           = shadowCullResult.meshes;
            }
        }
    }
    a_CullResult.Shrink();
}

SceneCullResult Scene::CullEntities(const Core::Frustum& a_Frustum, const SceneCullSettings& a_CullSettings) const
{
    SceneCullResult res;
    CullEntities(a_Frustum, a_CullSettings, res);
    return res;
}
};
