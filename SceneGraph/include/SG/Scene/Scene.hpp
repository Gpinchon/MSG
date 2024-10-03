/*
 * @Author: gpinchon
 * @Date:   2021-06-19 15:05:33
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-07-01 22:30:44
 */

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <ECS/Registry.hpp>
#include <SG/Component/Children.hpp>
#include <SG/Component/Transform.hpp>
#include <SG/Core/Inherit.hpp>
#include <SG/Core/Object.hpp>
#include <SG/Core/Property.hpp>
#include <SG/Core/Texture/TextureSampler.hpp>
#include <SG/Entity/NodeGroup.hpp>
#include <SG/Scene/Octree.hpp>

#include <memory>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG::Component {
class Frustum;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
struct CullResult {
    std::vector<ECS::DefaultRegistry::EntityRefType> entities;
    std::vector<ECS::DefaultRegistry::EntityRefType> meshes;
    std::vector<ECS::DefaultRegistry::EntityRefType> skins;
    std::vector<ECS::DefaultRegistry::EntityRefType> lights;
};

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
    inline auto& GetRootTransform()
    {
        return GetRootEntity().template GetComponent<Component::Transform>();
    }
    inline auto& GetRootChildren()
    {
        return GetRootEntity().template GetComponent<Component::Children>();
    }
    void UpdateOctree();
    void UpdateWorldTransforms() { Node::UpdateWorldTransform(GetRootEntity(), {}, true); }
    void UpdateBoundingVolumes();
    void CullEntities();
    CullResult CullEntities(const Component::Frustum& a_Frustum) const;
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
