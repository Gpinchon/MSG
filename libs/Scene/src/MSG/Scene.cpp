#include <MSG/Camera.hpp>
#include <MSG/Children.hpp>
#include <MSG/Light/PunctualLight.hpp>
#include <MSG/Mesh.hpp>
#include <MSG/Mesh/Skin.hpp>
#include <MSG/Scene.hpp>
#include <MSG/Tools/Debug.hpp>
#include <MSG/Transform.hpp>

#include <format>
#include <limits>
#include <ranges>

#include <glm/gtx/transform.hpp>

namespace MSG {
Scene::Scene()
    : Inherit()
{
    static auto s_sceneNbr { 0u };
    s_sceneNbr++;
    SetName(std::format("Scene_{}", s_sceneNbr));
}

template <bool Root, typename EntityRefType>
static BoundingVolume& UpdateBoundingVolume(
    EntityRefType& a_Entity,
    BoundingVolume& a_InfBV,
    std::vector<BoundingVolume*>& a_InfBVs)
{
    auto& bv           = a_Entity.template GetComponent<BoundingVolume>();
    auto& transform    = a_Entity.template GetComponent<Transform>();
    auto& transformMat = transform.GetWorldTransformMatrix();
    auto& transformPos = transform.GetWorldPosition();
    auto hasLight      = a_Entity.template HasComponent<PunctualLight>();
    auto hasMesh       = a_Entity.template HasComponent<Mesh>();
    auto hasMeshSkin   = a_Entity.template HasComponent<MeshSkin>();
    auto hasChildren   = a_Entity.template HasComponent<Children>();
    bv                 = { transformPos, { 0, 0, 0 } };
    if (hasMeshSkin) [[unlikely]] {
        auto& skin = a_Entity.template GetComponent<MeshSkin>();
        bv += skin.ComputeBoundingVolume();
    } else if (hasMesh) {
        auto& mesh = a_Entity.template GetComponent<Mesh>();
        bv += transformMat * mesh.geometryTransform * mesh.boundingVolume;
    }
    if (hasLight) [[unlikely]] {
        auto& light        = a_Entity.template GetComponent<PunctualLight>();
        auto lightHalfSize = light.GetHalfSize();
        bv += BoundingVolume(transformPos, lightHalfSize);
    }
    if (hasChildren) {
        for (auto& child : a_Entity.template GetComponent<Children>()) {
            auto& childBV = UpdateBoundingVolume<false>(child, a_InfBV, a_InfBVs);
            if (childBV.IsInf()) [[unlikely]] {
                a_InfBVs.emplace_back(&childBV);
                if constexpr (!Root)
                    bv += childBV;
            } else
                bv += childBV;
        }
    }
    if constexpr (!Root) {
        if (bv.IsInf()) [[unlikely]] {
            glm::vec3 center = bv.center;
            for (uint8_t i = 0u; i < 3; i++) {
                if (std::isinf(bv.halfSize[i]))
                    center[i] = transformPos[i];
            }
            a_InfBV += BoundingVolume(center, { 0, 0, 0 });
        }
    }
    return bv;
}

void Scene::UpdateBoundingVolumes()
{
    BoundingVolume infBV;
    std::vector<BoundingVolume*> infBVs;
    auto& newBV = UpdateBoundingVolume<true>(GetRootEntity(), infBV, infBVs);
    SetBoundingVolume(newBV + infBV);
    for (auto& bv : infBVs)
        *bv = GetBoundingVolume(); // set the infinite BV to the scene's BV
}

template <typename EntityRefType, typename OctreeType, typename OctreeRefType>
void InsertEntity(EntityRefType& a_Entity, OctreeType& a_Octree, const OctreeRefType& a_Ref)
{
    auto& bv = a_Entity.template GetComponent<BoundingVolume>();
    auto ref = a_Octree.Insert(a_Ref, a_Entity, bv);
    if (!ref.first)
        return;
    if (a_Entity.template HasComponent<Children>()) {
        for (auto& child : a_Entity.template GetComponent<Children>()) {
            InsertEntity(child, a_Octree, ref.second);
        }
    }
}

Transform& Scene::GetRootTransform()
{
    return GetRootEntity().GetComponent<Transform>();
}

Children& Scene::GetRootChildren()
{
    return GetRootEntity().GetComponent<Children>();
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
    const auto& mesh           = a_Entity.template GetComponent<Mesh>();
    const auto& bv             = a_Entity.template GetComponent<BoundingVolume>();
    const auto viewBV          = a_CameraVP * bv;
    const auto viewSphere      = (Sphere)viewBV;
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

static glm::vec3 GetBVClosestPoint(const BoundingVolume& a_BV, const Plane& a_Plane)
{
    auto nx                         = a_Plane.GetNormal().x > 0 ? 1 : 0;
    auto ny                         = a_Plane.GetNormal().y > 0 ? 1 : 0;
    auto nz                         = a_Plane.GetNormal().z > 0 ? 1 : 0;
    std::array<glm::vec3, 2> minMax = { a_BV.Min(), a_BV.Max() };
    return { minMax[nx].x, minMax[ny].y, minMax[nz].z };
}

static bool BVInsideFrustum(const BoundingVolume& a_BV, const CameraFrustum& a_Frustum)
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
    auto const& camera          = GetCamera().GetComponent<Camera>();
    auto const& cameraTransform = GetCamera().GetComponent<Transform>();
    auto frustum                = camera.projection.GetFrustum(cameraTransform);
    CullEntities(frustum, a_CullSettings, GetVisibleEntities());
}

MSG::SceneShadowViewport CullShadow(const Scene& a_Scene, const Transform& a_Transform, const CameraProjection& a_Proj)
{
    constexpr SceneCullSettings shadowCullSettings {
        .cullMeshSkins = false,
        .cullLights    = false,
        .cullShadows   = false
    };
    return {
        .projection = a_Proj,
        .viewMatrix = glm::inverse(a_Transform.GetWorldTransformMatrix()),
        .meshes     = a_Scene.CullEntities(a_Proj.GetFrustum(a_Transform), shadowCullSettings).meshes
    };
}

template <typename T>
void CullShadow(const Scene& a_Scene, SceneVisibleShadows& a_ShadowCaster, const T& a_Light)
{
    errorFatal("Shadow culling not managed for this type of light");
}

template <>
void CullShadow(const Scene& a_Scene, SceneVisibleShadows& a_ShadowCaster, const LightDirectional& a_Light)
{
    const auto& lightTransform = a_ShadowCaster.GetComponent<Transform>();
    const auto& lightBV        = a_ShadowCaster.GetComponent<BoundingVolume>();
    const auto lightView       = glm::inverse(lightTransform.GetWorldTransformMatrix());
    const auto AABB            = lightView * lightBV;
    const auto minOrtho        = AABB.Min();
    const auto maxOrtho        = AABB.Max();
    const CameraProjection lightProj {
        CameraProjectionOrthographic {
            .xmag  = AABB.halfSize.x,
            .ymag  = AABB.halfSize.y,
            .znear = minOrtho.z,
            .zfar  = (maxOrtho.z - minOrtho.z),
        }
    };
    a_ShadowCaster.viewports.emplace_back(CullShadow(a_Scene, lightTransform, lightProj));
}

template <>
void CullShadow(const Scene& a_Scene, SceneVisibleShadows& a_ShadowCaster, const LightPoint& a_Light)
{
    const auto& lightTransform = a_ShadowCaster.GetComponent<Transform>();
    const auto& lightPosition  = lightTransform.GetWorldPosition();
    const auto lightView       = glm::inverse(lightTransform.GetWorldTransformMatrix());
    const auto& lightRange     = a_Light.range;
    CameraProjection lightProj;
    if (lightRange == std::numeric_limits<float>::infinity()) {
        CameraProjectionPerspectiveInfinite proj;
        proj.fov         = 90;
        proj.aspectRatio = 1.f; // shadowmaps are square
        proj.znear       = 0.01f;
        lightProj        = proj;
    } else {
        CameraProjectionPerspective proj;
        proj.fov         = 90;
        proj.aspectRatio = 1.f; // shadowmaps are square
        proj.znear       = 0.01f;
        proj.zfar        = lightRange;
        lightProj        = proj;
    }
    const std::array<glm::quat, 6> rotations {
        glm::normalize(glm::quatLookAt(glm::vec3(1, 0, 0), glm::vec3(0, -1, 0))), // X+
        glm::normalize(glm::quatLookAt(glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0))), // X-
        glm::normalize(glm::quatLookAt(glm::vec3(0, 1, 0), glm::vec3(0, 0, 1))), // Y+
        glm::normalize(glm::quatLookAt(glm::vec3(0, -1, 0), glm::vec3(0, 0, -1))), // Y-
        glm::normalize(glm::quatLookAt(glm::vec3(0, 0, 1), glm::vec3(0, -1, 0))), // Z+
        glm::normalize(glm::quatLookAt(glm::vec3(0, 0, -1), glm::vec3(0, -1, 0))), // Z-
    };
    a_ShadowCaster.viewports.reserve(6);
    for (uint8_t i = 0u; i < 6; i++) {
        Transform sideTransform;
        sideTransform.SetLocalPosition(lightTransform.GetWorldPosition());
        sideTransform.SetLocalScale(lightTransform.GetWorldScale());
        sideTransform.SetLocalRotation(rotations.at(i));
        sideTransform.UpdateWorld();
        a_ShadowCaster.viewports.emplace_back(CullShadow(a_Scene, sideTransform, lightProj));
    }
}

template <>
void CullShadow(const Scene& a_Scene, SceneVisibleShadows& a_ShadowCaster, const LightSpot& a_Light)
{
    const auto& lightTransform = a_ShadowCaster.GetComponent<Transform>();
    const auto lightView       = glm::inverse(lightTransform.GetWorldTransformMatrix());
    const auto& lightRange     = a_Light.range;
    CameraProjection lightProj;
    if (lightRange == std::numeric_limits<float>::infinity()) {
        CameraProjectionPerspectiveInfinite proj;
        proj.fov         = a_Light.outerConeAngle * 2.f * (180.f / M_PIf);
        proj.aspectRatio = 1.f; // shadowmaps are square
        proj.znear       = 0.001f;
        lightProj        = proj;
    } else {
        CameraProjectionPerspective proj;
        proj.fov         = a_Light.outerConeAngle * 2.f * (180.f / M_PIf);
        proj.aspectRatio = 1.f; // shadowmaps are square
        proj.znear       = 0.001f;
        proj.zfar        = lightRange;
        lightProj        = proj;
    }
    a_ShadowCaster.viewports.emplace_back(CullShadow(a_Scene, lightTransform, lightProj));
}

void Scene::CullShadows(SceneCullResult& a_CullResult) const
{
    auto castsShadow = [](auto& a_Entity) { return std::visit([](const auto& lightData) { return lightData.shadowSettings.castShadow; }, a_Entity.template GetComponent<PunctualLight>()); };
    a_CullResult.shadows.reserve(a_CullResult.lights.size());
    for (auto& light : a_CullResult.lights) {
        if (!castsShadow(light)) [[likely]]
            continue;
        auto& shadowCaster    = a_CullResult.shadows.emplace_back(light);
        const auto& lightData = shadowCaster.GetComponent<PunctualLight>();
        std::visit([this, &shadowCaster](const auto& a_LightData) mutable {
            CullShadow(*this, shadowCaster, a_LightData);
        },
            lightData);
    }
}

void Scene::CullEntities(const CameraFrustum& a_Frustum, const SceneCullSettings& a_CullSettings, SceneCullResult& a_CullResult) const
{
    a_CullResult.Clear();
    auto const& camera           = GetCamera().GetComponent<Camera>();
    auto const& cameraTransform  = GetCamera().GetComponent<Transform>();
    auto const& cameraProjection = camera.projection.GetMatrix();
    auto const cameraView        = glm::inverse(cameraTransform.GetWorldTransformMatrix());
    auto const cameraVP          = cameraProjection * cameraView;

    auto hasPunctualLight = [](auto& a_Entity) { return a_Entity.template HasComponent<PunctualLight>(); };
    auto hasMesh          = [](auto& a_Entity) { return a_Entity.template HasComponent<Mesh>(); };
    auto hasMeshSkin      = [](auto& a_Entity) { return a_Entity.template HasComponent<MeshSkin>(); };

    auto sortByDistance = [&nearPlane = a_Frustum[CameraFrustumFace::Near]](auto& a_Lhs, auto& a_Rhs) {
        auto& lBv = a_Lhs.template GetComponent<BoundingVolume>();
        auto& rBv = a_Rhs.template GetComponent<BoundingVolume>();
        auto lCp  = GetBVClosestPoint(lBv, nearPlane);
        auto rCp  = GetBVClosestPoint(rBv, nearPlane);
        auto lDi  = std::abs(nearPlane.GetDistance(lCp));
        auto rDi  = std::abs(nearPlane.GetDistance(rCp));
        return lDi < rDi;
    };
    auto sortByPriority = [&sortByDistance](auto& a_Lhs, auto& a_Rhs) {
        auto& lPl = a_Lhs.template GetComponent<PunctualLight>();
        auto& rPl = a_Rhs.template GetComponent<PunctualLight>();
        auto lPr  = std::visit([](const auto& light) { return light.priority; }, lPl);
        auto rPr  = std::visit([](const auto& light) { return light.priority; }, rPl);
        if (lPr == rPr) [[likely]] // if priorities are equal, sort by distance
            return sortByDistance(a_Lhs, a_Rhs);
        return lPr > rPr;
    };
    auto cullVisitor = [this, &a_CullResult, &a_Frustum, &a_CullSettings, &cameraVP, hasMesh, hasPunctualLight](auto& node) mutable {
        if (node.Empty() || !BVInsideFrustum(node.Bounds(), a_Frustum))
            return false; // no other entities further down or we're outside frustum
        for (auto& entity : node.Storage()) {
            auto& bv = entity.template GetComponent<BoundingVolume>();
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
    if (a_CullSettings.cullMeshSkins) {
        a_CullResult.skins.reserve(a_CullResult.meshes.size());
        std::ranges::copy_if(a_CullResult.meshes, std::back_inserter(a_CullResult.skins), hasMeshSkin); // No need to sort again
    }

    std::ranges::sort(a_CullResult.lights, sortByPriority);
    if (a_CullSettings.cullShadows)
        CullShadows(a_CullResult);

    a_CullResult.Shrink();
}

SceneCullResult Scene::CullEntities(const CameraFrustum& a_Frustum, const SceneCullSettings& a_CullSettings) const
{
    SceneCullResult res;
    CullEntities(a_Frustum, a_CullSettings, res);
    return res;
}
};
