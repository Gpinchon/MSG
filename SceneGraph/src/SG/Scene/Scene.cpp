/*
 * @Author: gpinchon
 * @Date:   2021-06-19 15:04:37
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-06-19 15:04:37
 */

#include <SG/Component/Camera.hpp>
#include <SG/Component/Children.hpp>
#include <SG/Component/LevelOfDetails.hpp>
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

struct BVPair {
    const Component::BoundingVolume* parent;
    Component::BoundingVolume* child;
};

template <typename EntityRefType>
static const Component::BoundingVolume& UpdateBoundingVolume(
    EntityRefType& a_Entity,
    const Component::BoundingVolume& a_BaseBV,
    std::vector<BVPair>& a_InfiniteBV)
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
            bv += UpdateBoundingVolume(child, bv, a_InfiniteBV);
        }
    }
    // if this has an infinite BV, store it for later and return the parent's BV
    if (glm::any(glm::isinf(bv.halfSize))) {
        a_InfiniteBV.emplace_back(&a_BaseBV, &bv);
        return a_BaseBV;
    }
    return bv;
}

void Scene::UpdateBoundingVolumes()
{
    std::vector<BVPair> infiniteBV;
    SetBoundingVolume(UpdateBoundingVolume(GetRootEntity(), GetBoundingVolume(), infiniteBV));
    for (auto& bv : infiniteBV)
        *bv.child = *bv.parent; // set the infinite BV to the parent's
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

void Scene::UpdateLods()
{
    if (GetCamera().Empty()) {
        errorLog("Scene has no camera, cannot update lods.");
        return;
    }
    auto const& camera           = GetCamera().GetComponent<SG::Component::Camera>();
    auto const& cameraTransform  = GetCamera().GetComponent<SG::Component::Transform>();
    auto const& cameraView       = glm::inverse(cameraTransform.GetWorldTransformMatrix());
    auto const& cameraProjection = camera.projection.GetMatrix();
    auto const cameraVP          = cameraProjection * cameraView;
    for (auto& entity : GetVisibleEntities().meshes) {
        auto hasLods = entity.template HasComponent<Component::LevelOfDetails>();
        auto hasBV   = entity.template HasComponent<Component::BoundingVolume>();
        if (hasLods && hasBV) {
            auto& lods                 = entity.template GetComponent<Component::LevelOfDetails>();
            const auto& bv             = entity.template GetComponent<Component::BoundingVolume>();
            const auto viewBV          = cameraVP * bv;
            const auto viewSphere      = (Component::BoundingSphere)viewBV;
            const float screenCoverage = std::min(viewSphere.radius, 1.f);
            lods.currentLevel          = 0;
            for (uint8_t levelI = 0; levelI < lods.levels.size(); levelI++) {
                auto& level    = lods.levels.at(levelI);
                float coverage = lods.screenCoverage.at(levelI) + GetLevelOfDetailsBias();
                if (screenCoverage > coverage)
                    break;
                lods.currentLevel = levelI + 1;
            }
        }
    }
}

static glm::vec3 GetBVClosestPoint(const Component::BoundingVolume& a_BV, const Component::Plane& a_Plane)
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
    auto CullVisitor = [&visibleEntities = res.entities, &a_Frustum](auto& node) mutable {
        if (node.Empty() || !BVInsideFrustum(node.Bounds(), a_Frustum))
            return false;
        for (auto& entity : node.Storage()) {
            auto& bv = entity.template GetComponent<Component::BoundingVolume>();
            if (BVInsideFrustum(bv, a_Frustum))
                visibleEntities.push_back(entity);
        }
        return true;
    };
    GetOctree().Visit(CullVisitor);
    std::ranges::sort(res.entities, [&a_Frustum](auto& a_Lhs, auto& a_Rhs) {
        auto& fPl = a_Frustum[Component::FrustumFace::Near];
        auto& lBv = a_Lhs.template GetComponent<Component::BoundingVolume>();
        auto& rBv = a_Rhs.template GetComponent<Component::BoundingVolume>();
        auto lCp  = GetBVClosestPoint(lBv, fPl);
        auto rCp  = GetBVClosestPoint(rBv, fPl);
        auto lDi  = std::abs(fPl.GetDistance(lCp));
        auto rDi  = std::abs(fPl.GetDistance(rCp));
        return lDi < rDi;
    });
    std::ranges::copy_if(res.entities, std::back_inserter(res.lights), [](auto& a_Entity) {
        return a_Entity.template HasComponent<SG::Component::PunctualLight>();
    });
    std::ranges::copy_if(res.entities, std::back_inserter(res.meshes), [](auto& a_Entity) {
        return a_Entity.template HasComponent<SG::Component::Mesh>();
    });
    std::ranges::copy_if(res.meshes, std::back_inserter(res.skins), [](auto& a_Entity) {
        return a_Entity.template HasComponent<SG::Component::MeshSkin>();
    });
    return std::move(res);
}
};
