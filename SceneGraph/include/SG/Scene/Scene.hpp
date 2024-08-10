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
#include <SG/Entity/Node.hpp>

#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Scene : public Inherit<Object, Scene> {
    PROPERTY(std::shared_ptr<ECS::DefaultRegistry>, Registry, nullptr);
    /** @brief the camera the Scene will be seen from */
    PROPERTY(ECS::DefaultRegistry::EntityRefType, Camera, );
    PROPERTY(ECS::DefaultRegistry::EntityRefType, RootEntity, );
    PROPERTY(TextureSampler, Skybox, );
    PROPERTY(glm::vec3, BackgroundColor, 0, 0, 0);

public:
    Scene(const std::shared_ptr<ECS::DefaultRegistry>& a_ECSRegistry)
        : _Registry(a_ECSRegistry)
        , _RootEntity(a_ECSRegistry->CreateEntity<Component::Transform, Component::Children>())
    {
    }
    Scene(const std::shared_ptr<ECS::DefaultRegistry>& a_ECSRegistry, const std::string& name)
        : Scene(a_ECSRegistry)
    {
        SetName(name);
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

private:
    Scene();
};
};
