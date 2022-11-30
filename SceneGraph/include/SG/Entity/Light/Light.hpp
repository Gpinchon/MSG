/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-17 23:30:50
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Entity/Node/Node.hpp>
#include <SG/Component/Light/Light.hpp>

#include <glm/glm.hpp> // for glm::vec3
#include <memory> // for shared_ptr, weak_ptr
#include <string> // for string

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG::Light {
#define LIGHT_COMPONENTS NODE_COMPONENTS, SG::Light::Settings

uint32_t& GetNbr();
template<typename RegistryType>
auto Create(const RegistryType& a_Registry) {
    auto entity = Node::Create(a_Registry);
    entity.GetComponent<Component::Name>() = "Light_" + std::to_string(++GetNbr());
    entity.AddComponent<Component::Light>();
    return entity;
}
}
