#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Visitor/NodeVisitor.hpp>

#include <ostream>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Node;
class NodeGroup;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class SerializeVisitor : public NodeVisitor {
public:
    SerializeVisitor(std::ostream& a_Ostream, const Mode& a_Mode)
        : NodeVisitor(a_Mode)
        , _ostream(a_Ostream)
    {}

private:
    virtual void _Visit(Object& node) override final;
    virtual void _Visit(Node& a_Node) override final {
        _Visit(reinterpret_cast<Object&>(a_Node));
    }
    inline virtual void _Visit(NodeGroup& a_NodeGroup) override final {
        _Visit(reinterpret_cast<Node&>(a_NodeGroup));
    }
    std::ostream& _ostream;
};
}
