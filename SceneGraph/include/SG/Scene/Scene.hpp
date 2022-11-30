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
#include <SG/Core/Object.hpp>
#include <SG/Core/Inherit.hpp>
#include <SG/Core/Property.hpp>

#include <SG/Component/Transform.hpp>
#include <SG/Component/Children.hpp>

#include <ECS/Registry.hpp>

#include <vector>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Skybox;
};

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Scene : public Inherit<Object, Scene> {
    /** @brief the camera the Scene will be seen from */
    PROPERTY(ECS::DefaultRegistry::EntityRefType, Camera, );
    PROPERTY(std::shared_ptr<Skybox>, Skybox, nullptr);

public:
    Scene(const std::shared_ptr<ECS::DefaultRegistry>& a_ECSRegistry) : Scene() {
        _rootEntity = a_ECSRegistry->CreateEntity<Component::Transform, Component::Children>();
    }
    Scene(const std::shared_ptr<ECS::DefaultRegistry>& a_ECSRegistry, const std::string& name) : Scene(a_ECSRegistry) {
        SetName(name);
    }
    template<typename EntityRefType>
    inline void AddEntity(const EntityRefType& a_Entity) {
        SG::Node::SetParent(a_Entity, _rootEntity);
    }
    template<typename EntityRefType>
    inline void RemoveEntity(const EntityRefType& a_Entity) {
        SG::Node::RemoveParent(a_Entity, _rootEntity);
    }
    inline auto& GetRootTransform() {
        return _rootEntity.GetComponent<Component::Transform>();
    }
    inline auto& GetRootChildren() {
        return _rootEntity.GetComponent<Component::Children>();
    }

private:
    Scene();
    ECS::DefaultRegistry::EntityRefType _rootEntity;
};
};
