#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Core/Inherit.hpp>
#include <MSG/Core/Object.hpp>
#include <MSG/Core/Property.hpp>
#include <MSG/ECS/Registry.hpp>
#include <MSG/Entity/NodeGroup.hpp>
#include <MSG/Scene/CullResult.hpp>
#include <MSG/Scene/Octree.hpp>
#include <MSG/Texture/Sampler.hpp>
#include <MSG/ThreadPool.hpp>

#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
class Transform;
class CameraFrustum;
}

namespace MSG::Core {
class Children;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
class Scene : public Core::Inherit<Core::Object, Scene> {
    using OctreeType = SceneOctree<ECS::DefaultRegistry::EntityIDType, 2>;
    PROPERTY(std::shared_ptr<ECS::DefaultRegistry>, Registry, nullptr);
    /** @brief the camera the Scene will be seen from */
    PROPERTY(ECS::DefaultRegistry::EntityRefType, Camera, );
    PROPERTY(ECS::DefaultRegistry::EntityRefType, RootEntity, );
    PROPERTY(TextureSampler, Skybox, );
    PROPERTY(glm::vec3, BackgroundColor, 0, 0, 0);
    PROPERTY(BoundingVolume, BoundingVolume, { 0, 0, 0 }, { 100000, 100000, 100000 })
    PROPERTY(OctreeType, Octree, GetBoundingVolume());
    PROPERTY(SceneCullResult, VisibleEntities, );
    PROPERTY(float, LevelOfDetailsBias, 0);

public:
    Scene(const std::shared_ptr<ECS::DefaultRegistry>& a_ECSRegistry)
        : _Registry(a_ECSRegistry)
        , _RootEntity(Entity::NodeGroup::Create(a_ECSRegistry))
    {
    }
    Scene(const std::shared_ptr<ECS::DefaultRegistry>& a_ECSRegistry, const std::string& a_Name)
        : Scene(a_ECSRegistry)
    {
        SetName(a_Name);
    }
    template <typename EntityRefType>
    inline void AddEntity(const EntityRefType& a_Entity)
    {
        Entity::Node::SetParent(a_Entity, GetRootEntity());
    }
    template <typename EntityRefType>
    inline void RemoveEntity(const EntityRefType& a_Entity)
    {
        Entity::Node::RemoveParent(a_Entity, GetRootEntity());
    }
    Transform& GetRootTransform();
    Children& GetRootChildren();
    void UpdateOctree();
    void UpdateWorldTransforms() { Entity::Node::UpdateWorldTransform(GetRootEntity(), {}, true); }
    void UpdateBoundingVolumes();
    /** @brief culls the current visible entities and stores them inside VisibleEntities */
    void CullEntities(const SceneCullSettings& a_CullSettings = {});
    /**
     * @brief Culls the specified entities and stores them into a_Result.
     * The entities must have a BoundingVolume in order to be culled
     * @param a_Tp the threadpool the relevant tasks will be pushed to. Remenber to wait for the tasks to end afterwards.
     * @param a_Frustum the frustum to use for culling entities
     * @param a_CullSettings specifies which type of entities to cull
     * @param a_Result the output will be stored there because of performance and multithreading
     */
    void CullEntities(ThreadPool& a_Tp, const CameraFrustum& a_Frustum, const SceneCullSettings& a_CullSettings, SceneCullResult& a_Result) const;
    /**
     * @brief culls shadows using the result of CullEntities
     *
     * @param a_Tp the threadpool the relevant tasks will be pushed to. Remenber to wait for the tasks to end afterwards.
     * @param a_CullResult a cull result storing the meshes and lights, usually the output of CullEntities
     * @param a_Result the output will be stored there because of performance and multithreading
     */
    void CullShadows(ThreadPool& a_Tp, const SceneCullResult& a_CullResult, std::vector<SceneVisibleShadows>& a_Result) const;
    void Update()
    {
        UpdateWorldTransforms();
        UpdateBoundingVolumes();
        UpdateOctree();
        CullEntities();
        _cullingThreadpool.Wait();
    }

private:
    Scene();
    ThreadPool _cullingThreadpool;
};
};
