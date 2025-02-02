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
    using OctreeType = SceneOctree<ECS::DefaultRegistry::EntityRefType, 2>;
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
    void CullEntities(const SceneCullSettings& a_CullSettings = {});
    void CullEntities(const CameraFrustum& a_Frustum, const SceneCullSettings& a_CullSettings, SceneCullResult& a_CullResult) const;
    SceneCullResult CullEntities(const CameraFrustum& a_Frustum, const SceneCullSettings& a_CullSettings) const;
    void Update()
    {
        UpdateWorldTransforms();
        UpdateBoundingVolumes();
        UpdateOctree();
        CullEntities();
    }

private:
    Scene();
};
};
