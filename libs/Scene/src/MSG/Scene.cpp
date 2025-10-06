#include <MSG/Camera.hpp>
#include <MSG/Children.hpp>
#include <MSG/Debug.hpp>
#include <MSG/FogArea.hpp>
#include <MSG/Light/PunctualLight.hpp>
#include <MSG/Mesh.hpp>
#include <MSG/Mesh/Skin.hpp>
#include <MSG/Scene.hpp>
#include <MSG/Tools/MakeArrayHelper.hpp>
#include <MSG/Transform.hpp>

#include <format>
#include <limits>
#include <ranges>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

namespace MSG {
Scene::Scene()
    : Inherit()
{
    static auto s_sceneNbr { 0u };
    s_sceneNbr++;
    SetName(std::format("Scene_{}", s_sceneNbr));
}

#define FIX_INF_BV(a_Bv)                            \
    if constexpr (Root) {                           \
        for (uint8_t i = 0u; i < 3; i++) {          \
            if (std::isinf(a_Bv.halfSize[i])) {     \
                a_Bv.center[i]   = transformPos[i]; \
                a_Bv.halfSize[i] = 0.f;             \
            }                                       \
        }                                           \
    }

template <bool Root, typename EntityRefType>
static BoundingVolume& UpdateBoundingVolume(
    EntityRefType& a_Entity,
    BoundingVolume& a_MeshBV,
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
    auto hasFog        = a_Entity.template HasComponent<FogArea>();
    bv                 = { transformPos, { 0, 0, 0 } };
    if (hasMeshSkin) [[unlikely]] {
        auto& skin = a_Entity.template GetComponent<MeshSkin>();
        bv += skin.ComputeBoundingVolume();
        a_MeshBV += skin.ComputeBoundingVolume();
    } else if (hasMesh) {
        auto& mesh  = a_Entity.template GetComponent<Mesh>();
        auto meshBV = transformMat * mesh.geometryTransform * mesh.boundingVolume;
        bv += meshBV;
        a_MeshBV += meshBV;
    }
    if (hasLight) [[unlikely]] {
        auto& light  = a_Entity.template GetComponent<PunctualLight>();
        auto lightBV = BoundingVolume(transformPos, light.GetHalfSize());
        FIX_INF_BV(lightBV);
        bv += lightBV;
    }
    if (hasFog) [[unlikely]] {
        auto& fogArea = a_Entity.template GetComponent<FogArea>();
        BoundingVolume fogBV;
        fogBV.SetMinMax(
            fogArea.Min() + transformPos,
            fogArea.Max() + transformPos);
        FIX_INF_BV(fogBV);
        bv += fogBV;
    }
    if (hasChildren) [[likely]] {
        auto& children = a_Entity.template GetComponent<Children>();
        BoundingVolume childrenBV;
        for (auto& child : children) {
            auto childBV = UpdateBoundingVolume<false>(child, a_MeshBV, a_InfBV, a_InfBVs);
            FIX_INF_BV(childBV);
            childrenBV += childBV;
        }
        bv += childrenBV;
    }
    if constexpr (!Root) {
        bool isInf       = false;
        glm::vec3 center = bv.center;
        for (uint8_t i = 0u; i < 3; i++) {
            if (std::isinf(bv.halfSize[i])) {
                center[i] = transformPos[i];
                isInf     = true;
            }
        }
        if (isInf) {
            a_InfBV += BoundingVolume(center, { 0, 0, 0 });
            a_InfBVs.emplace_back(&bv);
        }
    }
    return bv;
}

void Scene::UpdateBoundingVolumes()
{
    BoundingVolume infBV;
    BoundingVolume meshBV;
    std::vector<BoundingVolume*> infBVs;
    auto& newBV = UpdateBoundingVolume<true>(GetRootEntity(), meshBV, infBV, infBVs);
    SetMeshBoundingVolume(meshBV);
    SetBoundingVolume(newBV + infBV);
    for (auto& bv : infBVs)
        *bv = GetBoundingVolume(); // set the infinite BV to the scene's BV
}

template <typename EntityRefType, typename BVHType>
void InsertEntity(EntityRefType& a_Entity, BVHType& a_BVH)
{
    a_BVH.InsertLeaf(a_Entity.template GetComponent<BoundingVolume>(), a_Entity);
    if (a_Entity.template HasComponent<Children>()) {
        for (auto& child : a_Entity.template GetComponent<Children>()) {
            InsertEntity(child, a_BVH);
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

void Scene::UpdateBVH()
{
    auto const& bv = GetBoundingVolume();
    // clear up BVH
    GetBVH().Clear();
    InsertEntity(GetRootEntity(), GetBVH());
}

template <typename RegistryType, typename EntityIDType>
auto ComputeLod(const RegistryType& a_Registry, const EntityIDType& a_EntityID, const glm::mat4x4& a_CameraVP, const float& a_LodBias)
{
    const auto& mesh           = a_Registry.template GetComponent<Mesh>(a_EntityID);
    const auto& bv             = a_Registry.template GetComponent<BoundingVolume>(a_EntityID);
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

MSG::SceneShadowViewport CullShadow(const Scene& a_Scene, const Transform& a_Transform, const CameraProjection& a_Proj)
{
    constexpr SceneCullSettings shadowCullSettings {
        .cullMeshSkins = false,
        .cullLights    = false,
        .cullShadows   = false,
        .cullFogAreas  = false
    };
    SceneCullResult cullResult;
    a_Scene.CullEntities(a_Proj.GetFrustum(a_Transform), shadowCullSettings, cullResult);
    return {
        .projection = a_Proj,
        .viewMatrix = glm::inverse(a_Transform.GetWorldTransformMatrix()),
        .meshes     = cullResult.meshes
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
    const auto& registry            = a_Scene.GetRegistry();
    const Transform& lightTransform = registry->GetComponent<Transform>(a_ShadowCaster);
    const glm::mat4x4 lightView     = glm::inverse(lightTransform.GetWorldTransformMatrix());
    // project the world space bounding volume to light space
    const bool infiniteLight          = glm::any(glm::isinf(a_Light.halfSize));
    const BoundingVolume& bv          = infiniteLight ? a_Scene.GetMeshBoundingVolume() : registry->GetComponent<BoundingVolume>(a_ShadowCaster);
    const BoundingVolume bvLightSpace = lightView * bv;
    const glm::vec3 minOrtho          = bvLightSpace.Min();
    const glm::vec3 maxOrtho          = bvLightSpace.Max();
    CameraProjection lightProj        = CameraProjectionOrthographic {
               .left   = minOrtho.x,
               .right  = maxOrtho.x,
               .bottom = minOrtho.y,
               .top    = maxOrtho.y,
               .znear  = -maxOrtho.z,
               .zfar   = -minOrtho.z,
    };
    a_ShadowCaster.viewports.emplace_back(CullShadow(a_Scene, lightTransform, lightProj));
}

template <>
void CullShadow(const Scene& a_Scene, SceneVisibleShadows& a_ShadowCaster, const LightPoint& a_Light)
{
    const auto& registry       = a_Scene.GetRegistry();
    const auto& lightTransform = registry->GetComponent<Transform>(a_ShadowCaster);
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
    const auto& registry       = a_Scene.GetRegistry();
    const auto& lightTransform = registry->GetComponent<Transform>(a_ShadowCaster);
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

void Scene::CullEntities(const SceneCullSettings& a_CullSettings)
{
    if (GetCamera().Empty()) {
        errorLog("Scene has no camera, cannot cull entities.");
        return;
    }
    auto const& camera          = GetCamera().GetComponent<Camera>();
    auto const& cameraTransform = GetCamera().GetComponent<Transform>();
    auto frustum                = camera.projection.GetFrustum(cameraTransform);
    GetVisibleEntities().Clear();
    CullEntities(frustum, a_CullSettings, GetVisibleEntities());
}

struct SceneCullVisitor {
    template <typename BVHType, typename BVHNodeType>
    bool operator()(const BVHType& a_BVH, const BVHNodeType& a_Node)
    {
        if (!BVInsideFrustum(a_Node.bounds, frustum))
            return false; // no other entities further down or we're outside frustum
        if (a_Node.objectIndex != -1u)
            entities.emplace_back(a_BVH.objects[a_Node.objectIndex]);
        return true;
    }
    const ECS::DefaultRegistry& registry;
    const CameraFrustum frustum;
    std::vector<VisibleEntity> entities;
};

template <typename T, typename Pred, typename... Args>
typename std::vector<T>::iterator EmplaceSorted(std::vector<T>& a_Vec, Pred a_Pred, Args&&... a_Args)
{
    return a_Vec.emplace(
        std::upper_bound(a_Vec.begin(), a_Vec.end(), std::forward<Args>(a_Args)..., a_Pred),
        std::forward<Args>(a_Args)...);
}

void Scene::CullEntities(const CameraFrustum& a_Frustum, const SceneCullSettings& a_CullSettings, SceneCullResult& a_Result) const
{
    auto const& registry         = *GetRegistry();
    auto const& camera           = GetCamera().GetComponent<Camera>();
    auto const& cameraTransform  = GetCamera().GetComponent<Transform>();
    auto const& cameraProjection = camera.projection.GetMatrix();
    auto const cameraView        = glm::inverse(cameraTransform.GetWorldTransformMatrix());
    auto const cameraVP          = cameraProjection * cameraView;

    auto hasPunctualLight = [&registry](auto& a_Entity) { return registry.HasComponent<PunctualLight>(a_Entity); };
    auto hasMesh          = [&registry](auto& a_Entity) { return registry.HasComponent<Mesh>(a_Entity); };
    auto hasMeshSkin      = [&registry](auto& a_Entity) { return registry.HasComponent<MeshSkin>(a_Entity); };
    auto hasFog           = [&registry](auto& a_Entity) { return registry.HasComponent<FogArea>(a_Entity); };

    auto sortByDistance = [&registry, &nearPlane = a_Frustum[CameraFrustumFace::Near]](auto& a_Lhs, auto& a_Rhs) {
        auto& lBv = registry.GetComponent<BoundingVolume>(a_Lhs);
        auto& rBv = registry.GetComponent<BoundingVolume>(a_Rhs);
        auto lCp  = GetBVClosestPoint(lBv, nearPlane);
        auto rCp  = GetBVClosestPoint(rBv, nearPlane);
        auto lDi  = std::abs(nearPlane.GetDistance(lCp));
        auto rDi  = std::abs(nearPlane.GetDistance(rCp));
        return lDi < rDi;
    };
    auto sortByPriority = [&registry, &sortByDistance](auto& a_Lhs, auto& a_Rhs) {
        auto& lPl = registry.GetComponent<PunctualLight>(a_Lhs);
        auto& rPl = registry.GetComponent<PunctualLight>(a_Rhs);
        auto lPr  = std::visit([](const auto& light) { return light.priority; }, lPl);
        auto rPr  = std::visit([](const auto& light) { return light.priority; }, rPl);
        return lPr > rPr;
    };
    SceneCullVisitor cullVisitor { registry, a_Frustum };
    GetBVH().Visit(cullVisitor);
    for (auto& entity : cullVisitor.entities)
        EmplaceSorted(a_Result.entities, sortByDistance, entity);
    // finalize culling
    for (auto& entity : a_Result.entities) {
        if (a_CullSettings.cullMeshes && hasMesh(entity)) {
            a_Result.meshes.emplace_back(entity, ComputeLod(registry, entity, cameraVP, GetLevelOfDetailsBias()));
            if (a_CullSettings.cullMeshSkins && hasMeshSkin(entity))
                a_Result.skins.emplace_back(entity);
        }
        if (a_CullSettings.cullFogAreas && hasFog(entity)) [[unlikely]]
            a_Result.fogAreas.emplace_back(entity);
        if (a_CullSettings.cullLights && hasPunctualLight(entity)) [[unlikely]]
            EmplaceSorted(a_Result.lights, sortByPriority, entity);
    }
    if (a_CullSettings.cullShadows)
        CullShadows(a_Result, a_CullSettings.maxShadows, a_Result.shadows);
    a_Result.Shrink();
}

void Scene::CullShadows(const SceneCullResult& a_CullResult, const uint32_t& a_MaxShadows, std::vector<SceneVisibleShadows>& a_Result) const
{
    auto const& registry = *GetRegistry();
    auto castsShadow     = [&registry](auto& a_Entity) { return std::visit([](const auto& lightData) { return lightData.shadowSettings.castShadow; }, registry.GetComponent<PunctualLight>(a_Entity)); };
    a_Result.reserve(a_MaxShadows);
    for (auto& light : a_CullResult.lights) {
        if (!castsShadow(light)) [[likely]]
            continue;
        if (a_Result.size() == a_MaxShadows)
            break;
        auto& shadowCaster    = a_Result.emplace_back(light);
        const auto& lightData = registry.GetComponent<PunctualLight>(shadowCaster);
        std::visit([this, &shadowCaster](const auto& a_LightData) mutable {
            CullShadow(*this, shadowCaster, a_LightData);
        },
            lightData);
    }
}
};
