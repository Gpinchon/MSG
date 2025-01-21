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
#include <Core/Light/PunctualLight.hpp>
#include <Entity/Node.hpp>

#include <glm/glm.hpp> // for glm::vec3
#include <memory> // for shared_ptr, weak_ptr
#include <string> // for string

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG::Entity::PunctualLight {
#define LIGHT_COMPONENTS NODE_COMPONENTS, Core::PunctualLight

uint32_t& GetNbr();
template <typename RegistryType>
auto Create(const RegistryType& a_Registry)
{
    auto entity                                = Node::Create(a_Registry);
    entity.template GetComponent<Core::Name>() = "PunctualLight_" + std::to_string(++GetNbr());
    entity.template AddComponent<Core::PunctualLight>();
    return entity;
}
}
