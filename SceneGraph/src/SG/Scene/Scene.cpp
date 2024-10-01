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

namespace TabGraph::SG {
Scene::Scene()
    : Inherit()
{
    static auto s_sceneNbr { 0u };
    SetName("Scene_" + std::to_string(++s_sceneNbr));
}

template <typename EntityRef>
Component::BoundingVolume& UpdateBoundingVolume(const EntityRef& a_Entity)
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
    GetBoundingVolume() += UpdateBoundingVolume(GetRootEntity());
    GetRootEntity().GetComponent<Component::BoundingVolume>() = GetBoundingVolume();
}

template <typename EntityRef>
void CullEntity(const EntityRef& a_Entity, const Component::Frustum& a_Frustum, std::set<EntityRef>& a_Visible)
{
    auto& bv            = a_Entity.GetComponent<Component::BoundingVolume>();
    auto& tr            = a_Entity.GetComponent<Component::Transform>();
    auto points         = bv.Points();
    glm::vec3 minMax[2] = { bv.Min(), bv.Max() };
    for (auto& plane : a_Frustum) {
        // glm::vec3 U(0, 1, 0);
        // glm::vec3 L(1, 0, 0);
        // glm::vec3 F(0, 0, 1);
        // glm::vec3 N = plane.normal;
        // float p     = (abs(glm::dot(L, N)) * bv.halfSize.x) + (abs(glm::dot(U, N)) * bv.halfSize.y) + (abs(glm::dot(F, N)) * bv.halfSize.z);
        // auto M      = (bv.Min() + bv.Max()) / 2.f;
        // if (plane.Distance(M) <= -p)
        //     return;

        auto nx             = plane.GetNormal().x > 0 ? 1 : 0;
        auto ny             = plane.GetNormal().y > 0 ? 1 : 0;
        auto nz             = plane.GetNormal().z > 0 ? 1 : 0;
        glm::vec3 minMax[2] = { bv.Min(), bv.Max() };
        glm::vec3 vn(minMax[nx].x, minMax[ny].y, minMax[nz].z);
        if (plane.GetDistance(vn) < 0)
            return;
    }
    a_Visible.insert(a_Entity);
    if (a_Entity.HasComponent<Component::Children>()) {
        for (auto& child : a_Entity.GetComponent<Component::Children>()) {
            CullEntity(child, a_Frustum, a_Visible);
        }
    }
}

void Scene::CullEntities()
{
    auto& camera          = GetCamera().GetComponent<SG::Component::Camera>();
    auto& cameraTransform = GetCamera().GetComponent<SG::Component::Transform>();
    auto frustum          = camera.projection.GetFrustum(cameraTransform);
    GetVisibleEntities().clear();
    CullEntity(GetRootEntity(), frustum, GetVisibleEntities());
}
};
