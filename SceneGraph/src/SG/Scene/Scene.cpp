/*
 * @Author: gpinchon
 * @Date:   2021-06-19 15:04:37
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-06-19 15:04:37
 */

#include <SG/Component/Camera.hpp>
#include <SG/Component/Children.hpp>
#include <SG/Component/Mesh.hpp>
#include <SG/Component/MeshSkin.hpp>
#include <SG/Scene/Scene.hpp>
#include <Tools/Debug.hpp>

namespace TabGraph::SG {
Scene::Scene()
    : Inherit()
{
    static auto s_sceneNbr { 0u };
    SetName("Scene_" + std::to_string(++s_sceneNbr));
}

template <typename EntityRefType>
Component::BoundingVolume& UpdateBoundingVolume(EntityRefType& a_Entity)
{
    auto& bv           = a_Entity.GetComponent<Component::BoundingVolume>();
    auto& transform    = a_Entity.GetComponent<Component::Transform>();
    auto& transformMat = transform.GetWorldTransformMatrix();
    auto hasMesh       = a_Entity.HasComponent<Component::Mesh>();
    auto hasMeshSkin   = a_Entity.HasComponent<Component::MeshSkin>();
    auto hasChildren   = a_Entity.HasComponent<Component::Children>();
    bv                 = { transform.GetWorldPosition(), { 0, 0, 0 } };
    if (hasMeshSkin) {
        auto& skin = a_Entity.GetComponent<Component::MeshSkin>();
        bv += skin.ComputeBoundingVolume();
    } else if (hasMesh) {
        auto& mesh = a_Entity.GetComponent<Component::Mesh>();
        bv += transformMat * mesh.geometryTransform * mesh.boundingVolume;
    }
    if (hasChildren) {
        for (auto& child : a_Entity.GetComponent<Component::Children>()) {
            bv += UpdateBoundingVolume(child);
        }
    }
    return bv;
}

void Scene::UpdateBoundingVolumes()
{
    SetBoundingVolume(UpdateBoundingVolume(GetRootEntity()));
}

template <typename EntityRefType, typename OctreeType>
void InsertEntity(EntityRefType& a_Entity, OctreeType& a_Octree)
{
    auto& bv = a_Entity.GetComponent<Component::BoundingVolume>();
    if (!a_Octree.Insert(a_Entity, bv))
        return;
    if (a_Entity.HasComponent<Component::Children>()) {
        for (auto& child : a_Entity.GetComponent<Component::Children>()) {
            InsertEntity(child, a_Octree);
        }
    }
}

void Scene::UpdateOctree()
{
    auto& bv = GetBoundingVolume();
    // clear up octree
    GetOctree().Clear();
    GetOctree().SetMinMax(bv.Min() - 0.1f, bv.Max() + 0.1f);
    InsertEntity(GetRootEntity(), GetOctree());
}

static bool BVInsideFrustum(const Component::BoundingVolume& a_BV, const Component::Frustum& a_Frustum)
{
    for (auto& plane : a_Frustum) {
        auto nx             = plane.GetNormal().x > 0 ? 1 : 0;
        auto ny             = plane.GetNormal().y > 0 ? 1 : 0;
        auto nz             = plane.GetNormal().z > 0 ? 1 : 0;
        glm::vec3 minMax[2] = { a_BV.Min(), a_BV.Max() };
        glm::vec3 vn(minMax[nx].x, minMax[ny].y, minMax[nz].z);
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
    auto& camera          = GetCamera().GetComponent<SG::Component::Camera>();
    auto& cameraTransform = GetCamera().GetComponent<SG::Component::Transform>();
    auto frustum          = camera.projection.GetFrustum(cameraTransform);
    SetVisibleEntities(CullEntities(frustum));
}

std::set<ECS::DefaultRegistry::EntityRefType> Scene::CullEntities(const Component::Frustum& a_Frustum) const
{
    std::set<ECS::DefaultRegistry::EntityRefType> visibleEntities;
    auto CullVisitor = [&visibleEntities, &a_Frustum](auto& node) mutable {
        if (node.empty || !BVInsideFrustum(node.bounds, a_Frustum))
            return false;
        for (auto& entity : node.storage) {
            auto& bv = entity.GetComponent<Component::BoundingVolume>();
            if (BVInsideFrustum(bv, a_Frustum))
                visibleEntities.insert(entity);
        }
        return true;
    };
    GetOctree().Visit(CullVisitor);
    return visibleEntities;
}
};
