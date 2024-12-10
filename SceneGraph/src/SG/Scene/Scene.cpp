/*
 * @Author: gpinchon
 * @Date:   2021-06-19 15:04:37
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-06-19 15:04:37
 */

#include <SG/Component/Camera.hpp>
#include <SG/Component/Children.hpp>
#include <SG/Component/Light/PunctualLight.hpp>
#include <SG/Component/Mesh.hpp>
#include <SG/Component/MeshSkin.hpp>
#include <SG/Scene/Scene.hpp>
#include <Tools/Debug.hpp>

#include <format>
#include <ranges>

namespace TabGraph::SG {
Scene::Scene()
    : Inherit()
{
    static auto s_sceneNbr { 0u };
    s_sceneNbr++;
    SetName(std::format("Scene_{}", s_sceneNbr));
}

template <bool Root, typename EntityRefType>
static Component::BoundingVolume& UpdateBoundingVolume(
    EntityRefType& a_Entity,
    const Component::BoundingVolume& a_BaseBV,
    std::vector<Component::BoundingVolume*>& a_InfiniteBV)
{
    auto& bv           = a_Entity.template GetComponent<Component::BoundingVolume>();
    auto& transform    = a_Entity.template GetComponent<Component::Transform>();
    auto& transformMat = transform.GetWorldTransformMatrix();
    auto hasLight      = a_Entity.template HasComponent<Component::PunctualLight>();
    auto hasMesh       = a_Entity.template HasComponent<Component::Mesh>();
    auto hasMeshSkin   = a_Entity.template HasComponent<Component::MeshSkin>();
    auto hasChildren   = a_Entity.template HasComponent<Component::Children>();
    bv                 = { transform.GetWorldPosition(), { 0, 0, 0 } };
    bv = { transform.GetWorldPosition(), { 0, 0, 0 } };
    if (hasMeshSkin) {
        auto& skin = a_Entity.template GetComponent<Component::MeshSkin>();
        bv += skin.ComputeBoundingVolume();
    } else if (hasMesh) {
        auto& mesh = a_Entity.template GetComponent<Component::Mesh>();
        bv += transformMat * mesh.geometryTransform * mesh.boundingVolume;
    }
    if (hasLight) {
        auto& light        = a_Entity.GetComponent<Component::PunctualLight>();
        auto lightHalfSize = light.GetHalfSize();
        bv += Component::BoundingVolume(transform.GetWorldPosition(), lightHalfSize);
    }
    if (hasChildren) {
        for (auto& child : a_Entity.template GetComponent<Component::Children>()) {
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
    std::vector<Component::BoundingVolume*> infiniteBV;
    auto& newBV = UpdateBoundingVolume<true>(GetRootEntity(), GetBoundingVolume(), infiniteBV);
    SetBoundingVolume(newBV);
    for (auto& bv : infiniteBV)
        *bv = GetBoundingVolume(); // set the infinite BV to the scene's BV
}

template <typename EntityRefType, typename OctreeType, typename OctreeRefType>
void InsertEntity(EntityRefType& a_Entity, OctreeType& a_Octree, const OctreeRefType& a_Ref)
{
    auto& bv = a_Entity.template GetComponent<Component::BoundingVolume>();
    auto ref = a_Octree.Insert(a_Ref, a_Entity, bv);
    if (!ref.first)
        return;
    if (a_Entity.template HasComponent<Component::Children>()) {
        for (auto& child : a_Entity.template GetComponent<Component::Children>()) {
            InsertEntity(child, a_Octree, ref.second);
        }
    }
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
    const auto& mesh           = a_Entity.template GetComponent<Component::Mesh>();
    const auto& bv             = a_Entity.template GetComponent<Component::BoundingVolume>();
    const auto viewBV          = a_CameraVP * bv;
    const auto viewSphere      = (SG::Sphere)viewBV;
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

static glm::vec3 GetBVClosestPoint(const Component::BoundingVolume& a_BV, const SG::Plane& a_Plane)
{
    auto nx                         = a_Plane.GetNormal().x > 0 ? 1 : 0;
    auto ny                         = a_Plane.GetNormal().y > 0 ? 1 : 0;
    auto nz                         = a_Plane.GetNormal().z > 0 ? 1 : 0;
    std::array<glm::vec3, 2> minMax = { a_BV.Min(), a_BV.Max() };
    return { minMax[nx].x, minMax[ny].y, minMax[nz].z };
}

static bool BVInsideFrustum(const Component::BoundingVolume& a_BV, const Component::Frustum& a_Frustum)
{
    for (auto& plane : a_Frustum) {
        auto vn = GetBVClosestPoint(a_BV, plane);
        if (plane.GetDistance(vn) < 0) {
            return false;
        }
    }
    return true;
}

void Scene::CullEntities()
{
    if (GetCamera().Empty()) {
        errorLog("Scene has no camera, cannot cull entities.");
        return;
    }
    auto const& camera          = GetCamera().GetComponent<SG::Component::Camera>();
    auto const& cameraTransform = GetCamera().GetComponent<SG::Component::Transform>();
    auto frustum                = camera.projection.GetFrustum(cameraTransform);
    SetVisibleEntities(CullEntities(frustum));
}

CullResult Scene::CullEntities(const Component::Frustum& a_Frustum) const
{
    CullResult res;
    auto sortByDistance = [&nearPlane = a_Frustum[Component::FrustumFace::Near]](auto& a_Lhs, auto& a_Rhs) {
        auto& lBv = a_Lhs.template GetComponent<Component::BoundingVolume>();
        auto& rBv = a_Rhs.template GetComponent<Component::BoundingVolume>();
        auto lCp  = GetBVClosestPoint(lBv, nearPlane);
        auto rCp  = GetBVClosestPoint(rBv, nearPlane);
        auto lDi  = std::abs(nearPlane.GetDistance(lCp));
        auto rDi  = std::abs(nearPlane.GetDistance(rCp));
        return lDi < rDi;
    };
    auto sortByPriority = [&sortByDistance](auto& a_Lhs, auto& a_Rhs) {
        auto& lPl = a_Lhs.template GetComponent<SG::Component::PunctualLight>();
        auto& rPl = a_Rhs.template GetComponent<SG::Component::PunctualLight>();
        auto lPr  = std::visit([](const auto& light) { return light.priority; }, lPl);
        auto rPr  = std::visit([](const auto& light) { return light.priority; }, rPl);
        if (lPr == rPr) // if priorities are equal, sort by distance
            return sortByDistance(a_Lhs, a_Rhs);
        return lPr > rPr;
    };
    auto cullVisitor = [&visibleEntities = res.entities, &a_Frustum](auto& node) mutable {
        if (node.Empty() || !BVInsideFrustum(node.Bounds(), a_Frustum))
            return false; // no other entities further down or we're outside frustum
        for (auto& entity : node.Storage()) {
            auto& bv = entity.template GetComponent<Component::BoundingVolume>();
            if (BVInsideFrustum(bv, a_Frustum))
                visibleEntities.push_back(entity);
        }
        return true;
    };

    GetOctree().Visit(cullVisitor);
    auto const& camera           = GetCamera().GetComponent<SG::Component::Camera>();
    auto const& cameraTransform  = GetCamera().GetComponent<SG::Component::Transform>();
    auto const& cameraView       = glm::inverse(cameraTransform.GetWorldTransformMatrix());
    auto const& cameraProjection = camera.projection.GetMatrix();
    auto const cameraVP          = cameraProjection * cameraView;

    res.lights.reserve(res.entities.size());
    std::ranges::copy_if(res.entities, std::back_inserter(res.lights), [](auto& a_Entity) {
        return a_Entity.template HasComponent<SG::Component::PunctualLight>();
    });
    std::ranges::sort(res.lights, sortByPriority);
    res.meshes.reserve(res.entities.size());
    for (auto& entity : res.entities) {
        if (entity.HasComponent<SG::Component::Mesh>())
            res.meshes.emplace_back(entity, ComputeLod(entity, cameraVP, GetLevelOfDetailsBias()));
    }
    std::ranges::sort(res.meshes, sortByDistance);
    res.lights.reserve(res.meshes.size());
    std::ranges::copy_if(res.meshes, std::back_inserter(res.skins), [](auto& a_Entity) {
        return a_Entity.template HasComponent<SG::Component::MeshSkin>();
    }); // No need to sort again

    return res;
}
};
