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
#include <SG/Node/NodeGroup.hpp>
#include <SG/Core/Inherit.hpp>
#include <SG/Core/Property.hpp>

#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Animation;
class Light;
class Skybox;
};

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Scene : public Inherit<NodeGroup, Scene> {
    /** @brief the camera the Scene will be seen from */
    PROPERTY(ECS::DefaultRegistry::EntityRefType, Camera, );
    PROPERTY(std::shared_ptr<Skybox>, Skybox, nullptr);
public:
    Scene();
    Scene(const std::shared_ptr<ECS::DefaultRegistry>& a_ECSRegistry) : Scene() {
        _rootEntity = SG::CreateNodeGroup(a_ECSRegistry);
    }
    Scene(const std::string& name)
        : Scene()
    {
        SetName(name);
    }
    template<typename EntityRefType>
    inline void AddEntity(const EntityRefType& a_Entity) {
        SG::NodeSetParent(a_Entity, _rootEntity);
    }
    template<typename EntityRefType>
    inline void RemoveEntity(const EntityRefType& a_Entity) {
        SG::NodeRemoveParent(a_Entity, _rootEntity);
    }

    inline void AddNode(std::shared_ptr<Node> node)
    {
        node->SetParent(std::static_pointer_cast<NodeGroup>(shared_from_this()));
    }
    inline void RemoveNode(std::shared_ptr<Node> node)
    {
        if (node->GetParent().get() == this)
            node->SetParent(nullptr);
    }
    inline auto GetAnimations() const {
        return _animations;
    }
    inline auto GetLights() const {
        return _lights;
    }
    inline auto AddAnimation(std::shared_ptr<Animation> animation) {
        _animations.push_back(animation);
    }
    inline auto AddLight(std::shared_ptr<Light> light) {
        _lights.push_back(light);
    }

private:
    std::vector<std::shared_ptr<Animation>> _animations;
    std::vector<std::shared_ptr<Light>> _lights;
    ECS::DefaultRegistry::EntityRefType _rootEntity;
};
};
