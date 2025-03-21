#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Entity/Node.hpp>
#include <MSG/Light/PunctualLight.hpp>

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
#define LIGHT_COMPONENTS NODE_COMPONENTS, PunctualLight

uint32_t& GetNbr();
template <typename RegistryType>
auto Create(const RegistryType& a_Registry)
{
    auto entity                                = Node::Create(a_Registry);
    entity.template GetComponent<Core::Name>() = "PunctualLight_" + std::to_string(++GetNbr());
    entity.template AddComponent<MSG::PunctualLight>();
    return entity;
}
}
