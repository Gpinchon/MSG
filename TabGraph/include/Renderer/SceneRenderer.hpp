/*
* @Author: gpinchon
* @Date:   2021-03-25 11:04:30
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-28 07:55:36
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <glm/fwd.hpp>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph {
namespace Nodes {
class Scene;
}
}

namespace TabGraph::Renderer {
class SceneRenderer;
struct Options;
SceneRenderer& GetRenderer(std::shared_ptr<Nodes::Scene> scene);
void OnFrameBegin(std::shared_ptr<Nodes::Scene> scene, const Options& options);
void Render(std::shared_ptr<Nodes::Scene> scene, const Options& options);
void Render(std::shared_ptr<Nodes::Scene> scene, const Options& options, const glm::mat4& rootMatrix);
void OnFrameEnd(std::shared_ptr<Nodes::Scene> scene, const Options& options);
};
