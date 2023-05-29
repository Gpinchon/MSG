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
#include <SG/Entity/Node.hpp>

#include <SG/Component/Children.hpp>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG::NodeGroup {
#define NODEGROUP_COMPONENTS NODE_COMPONENTS, SG::Component::Children
/** @return the total nbr of Nodes created since start-up */
uint32_t& GetNbr();
template <typename RegistryType>
auto Create(const RegistryType& a_Registry)
{
    auto entity                                = SG::Node::Create(a_Registry);
    entity.GetComponent<SG::Component::Name>() = "NodeGroup_" + std::to_string(++GetNbr());
    entity.AddComponent<SG::Component::Children>();
    return entity;
}
}
