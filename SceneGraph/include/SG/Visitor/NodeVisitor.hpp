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
    virtual void operator()(Node& node) final
    {
        _Visit(node);
        _Traverse(node);
    };
    virtual void operator()(Renderable& node) final
    {
        _Visit(reinterpret_cast<Node&>(node));
        _Traverse(node);
    };
    virtual void operator()(NodeGroup& node) final
    {
        _Visit(reinterpret_cast<Node&>(node));
        _Traverse(node);
    };

    const Mode mode;

private:
    virtual void _Visit(Node& node) = 0;
    virtual void _Visit(NodeGroup& node) = 0;
    virtual void _Visit(Renderable& node) = 0;
    void _Traverse(Node& node);
    void _Traverse(NodeGroup& group);
    void _Traverse(Renderable& group);
};
}
