#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <ECS/Registry.hpp>
#include <SG/Scene/CullResult.hpp>
#include <SG/Core/Inherit.hpp>
#include <SG/Core/Object.hpp>
#include <SG/Core/Property.hpp>
#include <SG/Core/Texture/TextureSampler.hpp>
#include <SG/Entity/NodeGroup.hpp>
#include <SG/Scene/Octree.hpp>

#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Frustum;
}
namespace TabGraph::SG::Component {
class Transform;
class Children;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Scene : public Inherit<Object, Scene> {
    using OctreeType = Octree<ECS::DefaultRegistry::EntityRefType, 2>;
    PROPERTY(std::shared_ptr<ECS::DefaultRegistry>, Registry, nullptr);
    /** @brief the camera the Scene will be seen from */
    PROPERTY(ECS::DefaultRegistry::EntityRefType, Camera, );
    PROPERTY(ECS::DefaultRegistry::EntityRefType, RootEntity, );
    PROPERTY(TextureSampler, Skybox, );
    PROPERTY(glm::vec3, BackgroundColor, 0, 0, 0);
    PROPERTY(Component::BoundingVolume, BoundingVolume, { 0, 0, 0 }, { 100000, 100000, 100000 })
    PROPERTY(OctreeType, Octree, GetBoundingVolume());
    PROPERTY(CullResult, VisibleEntities, );
    PROPERTY(float, LevelOfDetailsBias, 0);

public:
    Scene(const std::shared_ptr<ECS::DefaultRegistry>& a_ECSRegistry)
        : _Registry(a_ECSRegistry)
        , _RootEntity(NodeGroup::Create(a_ECSRegistry))
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
        SG::Node::SetParent(a_Entity, GetRootEntity());
    }
    template <typename EntityRefType>
    inline void RemoveEntity(const EntityRefType& a_Entity)
    {
        SG::Node::RemoveParent(a_Entity, GetRootEntity());
    }
    Component::Transform& GetRootTransform();
    Component::Children& GetRootChildren();
    void UpdateOctree();
    void UpdateWorldTransforms() { Node::UpdateWorldTransform(GetRootEntity(), {}, true); }
    void UpdateBoundingVolumes();
    void CullEntities();
    CullResult CullEntities(const Frustum& a_Frustum) const;
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
