/*
* @Author: gpinchon
* @Date:   2021-06-29 22:46:04
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:30:44
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Core/Inherit.hpp>
#include <Core/Object.hpp>

#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Renderer {
struct ShapeRenderer;
struct ShapeRendererDeleter {
    void operator()(ShapeRenderer* p) const;

};
};

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Shapes {
class Shape : public Core::Inherit<Core::Object, Shape> {
public:
    Shape(const std::string& name) : Inherit(name) {}
    Shape(const Shape& other)
        : Inherit(other)
        , _renderer(nullptr)
    {}
    ~Shape();
    Renderer::ShapeRenderer& GetRenderer()
    {
        return *_renderer;
    }

protected:
    std::unique_ptr<Renderer::ShapeRenderer, Renderer::ShapeRendererDeleter> _renderer;
};
};
