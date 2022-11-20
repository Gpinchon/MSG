#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Visitor/Visitor.hpp>

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
class NodeVisitor : public Visitor {
public:
    enum class Mode {
        VisitOnce, //Visit only one node
        VisitParents, //Visit parents until we encounter a node with no parent
        VisitChildren //Visit children until we encounter a node with no children
    };
    NodeVisitor(const Mode& a_Mode)
        : mode(a_Mode)
    {
    }
    //Using NVI as this might be faster in some case and to enforce Visit->Traverse order
    virtual void operator()(Object& a_Object) final
    {
        _Visit(a_Object);
    };
    virtual void operator()(Node& a_Node) final
    {
        _Visit(a_Node);
        _Traverse(a_Node);
    };
    virtual void operator()(NodeGroup& a_NodeGroup) final
    {
        _Visit(reinterpret_cast<Node&>(a_NodeGroup));
        _Traverse(a_NodeGroup);
    };

    const Mode mode;

private:
    virtual void _Visit(Object& node) = 0;
    virtual void _Visit(Node& node) = 0;
    virtual void _Visit(NodeGroup& node) = 0;
    void _Traverse(Node& node);
    void _Traverse(NodeGroup& group);
};
}
