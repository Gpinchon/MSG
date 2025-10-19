#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Core/Orientation.hpp>
#include <MSG/Entity/Node.hpp>
#include <MSG/FogArea.hpp>

#include <array>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/vec3.hpp>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace Msg::Entity::FogArea {
#define FOG_AREA_COMPONENTS NODE_COMPONENTS, FogArea
/** @return the total nbr of FogAreas created since start-up */
uint32_t& GetNbr();
template <typename RegistryType>
auto Create(const RegistryType& a_Registry)
{
    auto entity                                = Entity::Node::Create(a_Registry);
    entity.template GetComponent<Core::Name>() = "FogAreaRoot_" + std::to_string(++GetNbr());
    entity.template AddComponent<Msg::FogArea>();
    return entity;
}
}
