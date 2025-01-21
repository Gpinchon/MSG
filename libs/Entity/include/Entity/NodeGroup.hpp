/*
 * @Author: gpinchon
 * @Date:   2021-07-04 16:32:20
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-07-04 16:37:49
 */
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Entity/Node.hpp>

#include <Core/Children.hpp>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace MSG::Entity::NodeGroup {
#define NODEGROUP_COMPONENTS NODE_COMPONENTS, Core::Children
/** @return the total nbr of Nodes created since start-up */
uint32_t& GetNbr();
template <typename RegistryType>
auto Create(const RegistryType& a_Registry)
{
    auto entity                                = Entity::Node::Create(a_Registry);
    entity.template GetComponent<Core::Name>() = "NodeGroup_" + std::to_string(++GetNbr());
    entity.template AddComponent<Core::Children>();
    return entity;
}
}
