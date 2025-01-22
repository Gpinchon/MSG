#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Core/Inherit.hpp>
#include <Core/Object.hpp>
#include <Core/Property.hpp>
#include <Core/Texture/TextureSampler.hpp>
#include <ECS/Registry.hpp>
#include <Entity/NodeGroup.hpp>
#include <Scene/CullResult.hpp>
#include <Scene/Octree.hpp>

#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG::Core {
class Transform;
class Children;
class Frustum;
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
    PROPERTY(Core::TextureSampler, Skybox, );
    PROPERTY(glm::vec3, BackgroundColor, 0, 0, 0);
    PROPERTY(Core::BoundingVolume, BoundingVolume, { 0, 0, 0 }, { 100000, 100000, 100000 })
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
    Core::Transform& GetRootTransform();
    Core::Children& GetRootChildren();
    void UpdateOctree();
    void UpdateWorldTransforms() { Entity::Node::UpdateWorldTransform(GetRootEntity(), {}, true); }
    void UpdateBoundingVolumes();
    void CullEntities(const SceneCullSettings& a_CullSettings = {});
    void CullEntities(const Core::Frustum& a_Frustum, const SceneCullSettings& a_CullSettings, SceneCullResult& a_CullResult) const;
    SceneCullResult CullEntities(const Core::Frustum& a_Frustum, const SceneCullSettings& a_CullSettings) const;
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
