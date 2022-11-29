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
#include <SG/Node/Node.hpp>
#include <SG/Node/Children.hpp>

#include <SG/Core/Inherit.hpp>

#include <set>
#include <memory>
#include <string>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
#define NODEGROUP_COMPONENTS NODE_COMPONENTS, SG::Children
/** @return the total nbr of Nodes created since start-up */
uint32_t& GetNodeGroupNbr();
template<typename RegistryType>
auto CreateNodeGroup(const RegistryType& a_Registry) {
    auto entity = SG::CreateNode(a_Registry);
    entity.GetComponent<SG::Name>() = "NodeGroup_" + std::to_string(++GetNodeGroupNbr());
    entity.AddComponent<SG::Children>();
    return entity;
}

/**
 * @brief Describes a node group, can have children and a parent
*/
class NodeGroup : public Inherit<Node, NodeGroup> {
public:
    NodeGroup();
    NodeGroup(const std::string& a_Name)
        : Inherit(a_Name)
    {}
    NodeGroup(const NodeGroup& a_Group)
        : Inherit(a_Group)
    {
        //nodes group cannot have the children of another group
        _children.clear();
    }
    /** @return true if @arg child is in children list */
    inline auto HasChild(std::shared_ptr<Node> a_Child)
    {
        return std::find(_children.begin(), _children.end(), a_Child) != _children.end();
    };
    /** @return the number of children */
    inline auto ChildCount() const
    {
        return _children.size();
    };
    /** @return all the children */
    inline auto& GetChildren() const
    {
        return _children;
    }
    /** removes parenting for all children */
    inline void ClearChildren()
    {
        auto children{ _children };
        for (auto& child : children)
            child->SetParent(nullptr);
        _children.clear();
    }

private:
    friend Node;
    inline void _AddChild(std::shared_ptr<Node> child)
    {
        _children.insert(child);
    }
    /** removes parenting for specified @arg child */
    inline void _RemoveChild(std::shared_ptr<Node> child)
    {
        _children.erase(child);
    }
    std::set<std::shared_ptr<Node>> _children;
};
}
