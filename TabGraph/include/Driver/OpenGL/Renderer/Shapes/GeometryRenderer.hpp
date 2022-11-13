/*
* @Author: gpinchon
* @Date:   2021-03-22 20:48:26
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-26 11:50:13
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include "Renderer/Shapes/ShapeRenderer.hpp"

#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Shapes {
class Geometry;
}
namespace OpenGL {
class VertexArray;
}

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Renderer {
class GeometryRenderer : public ShapeRenderer {
public:
    GeometryRenderer(Shapes::Geometry& geometry);
    GeometryRenderer(const GeometryRenderer&) = delete;
    ~GeometryRenderer();
    void OnFrameBegin(const Options& options);
    void Render(bool doubleSided = false);
    virtual void Render(const Options& options, const glm::mat4& parentTransform, const glm::mat4& parentLastTransform) override;
    void OnFrameEnd(const Options& options);

private:
    Shapes::Geometry& _geometry;
    std::unique_ptr<OpenGL::VertexArray> _vao;
};
}
