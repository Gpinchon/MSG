/*
* @Author: gpinchon
* @Date:   2021-05-12 13:38:40
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-12 15:59:18
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Core/Inherit.hpp>
#include <Core/Object.hpp>
#include <glm/fwd.hpp>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Shapes {
class Shape;
}

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Renderer {
struct Options;
struct ShapeRenderer : Core::Inherit<Core::Object, ShapeRenderer> {
    virtual void OnFrameBegin(const Options& options) = 0;
    virtual void Render(const Options& options, const glm::mat4& parentTransform, const glm::mat4& parentLastTransform) = 0;
    virtual void OnFrameEnd(const Options& options) = 0;
};
void OnFrameBegin(std::shared_ptr<Shapes::Shape> surface, const Options& options);
void Render(std::shared_ptr<Shapes::Shape> surface, const Options& options);
void Render(std::shared_ptr<Shapes::Shape> surface, const Options& options, const glm::mat4& parentTransform);
void Render(std::shared_ptr<Shapes::Shape> surface, const Options& options, const glm::mat4& parentTransform, const glm::mat4& parentLastTransform);
void OnFrameEnd(std::shared_ptr<Shapes::Shape> surface, const Options& options);
};