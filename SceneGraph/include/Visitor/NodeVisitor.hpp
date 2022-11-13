#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Visitor/Visitor.hpp>

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
    const Mode mode;
    //Using NVI as this might be faster in some case and to enforce Visit->Traverse order
    virtual void operator()(Nodes::Node& node) final
    {
        _Visit(node);
        _Traverse(node);
    };
    virtual void operator()(Nodes::Renderable& node) final
    {
        _Visit(reinterpret_cast<Nodes::Node&>(node));
        _Traverse(node);
    };
    virtual void operator()(Nodes::Group& node) final
    {
        _Visit(reinterpret_cast<Nodes::Node&>(node));
        _Traverse(node);
    };

private:
    virtual void _Visit(Nodes::Node& node) = 0;
    virtual void _Visit(Nodes::Group& node) = 0;
    virtual void _Visit(Nodes::Renderable& node) = 0;
    void _Traverse(Nodes::Node& node);
    void _Traverse(Nodes::Group& group);
    void _Traverse(Nodes::Renderable& group);
};
};
