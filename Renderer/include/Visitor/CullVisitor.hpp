/*
* @Author: gpinchon
* @Date:   2021-06-20 13:22:20
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-26 23:02:04
*/
#pragma once
#include <Renderer/Renderer.hpp>
#include <Visitor/NodeVisitor.hpp>

#include <functional>
#include <typeindex>
#include <variant>
#include <vector>

namespace TabGraph::SG {
class Renderable;
};

namespace TabGraph::Renderer {
class CullVisitor : public SG::NodeVisitor {
public:
    CullVisitor(const Renderer::Options& options, const NodeVisitor::Mode& mode)
        : NodeVisitor(mode)
        , _options(options)
    {
    }
    inline const auto& GetOptions() const
    {
        return _options;
    }
    inline const auto& GetResult() const
    {
        return _result;
    }
    
private:
    const Renderer::Options _options;
    std::vector<Renderer::ShapeState> _result;

    // Hérité via NodeVisitor
    virtual void _Visit(Node& node) override;
    virtual void _Visit(NodeGroup& node) override;
    virtual void _Visit(Renderable& node) override;
};
};