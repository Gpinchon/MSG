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
#include <SG/Entity/Node/Node.hpp>

#include <SG/Component/Node/Children.hpp>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG::Node::Group {
#define NODEGROUP_COMPONENTS NODE_COMPONENTS, SG::Children
/** @return the total nbr of Nodes created since start-up */
uint32_t& GetNbr();
template<typename RegistryType>
auto Create(const RegistryType& a_Registry) {
    auto entity = SG::CreateNode(a_Registry);
    entity.GetComponent<SG::Name>() = "NodeGroup_" + std::to_string(++GetNbr());
    entity.AddComponent<SG::Children>();
    return entity;
}
}
