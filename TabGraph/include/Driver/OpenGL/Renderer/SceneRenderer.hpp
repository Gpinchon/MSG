/*
* @Author: gpinchon
* @Date:   2021-03-25 11:08:25
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-26 11:50:03
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Renderer/SceneRenderer.hpp>
#include <Renderer/Renderer.hpp>
#include <Light/LightProbe.hpp>

#include <glm/mat4x4.hpp>
#include <array>
#include <memory>
#include <vector>
#include <map>
#include <set>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph {
class Shape;
namespace Nodes {
class Node;
class Scene;
}
namespace Lights {
class Probe;
class ProbeGroup;
}
namespace Renderer {
struct Options;
}
}

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Renderer {
class SceneRenderer {
public:
    SceneRenderer(Nodes::Scene&);
    SceneRenderer(SceneRenderer&) = delete;
    void OnFrameBegin(const Renderer::Options& options);
    void Render(const Renderer::Options& options, const glm::mat4& rootMatrix);
    void OnFrameEnd(const Renderer::Options& options);
    TabGraph::Lights::Probe& GetClosestLightProbe(const glm::vec3& position);

private:
    Nodes::Scene& _scene;
    std::vector<ShapeState> _renderList;
    std::map<std::weak_ptr<Shapes::Shape>, glm::mat4, std::owner_less<>> _shapeLastTransform;
    std::set<std::weak_ptr<Shapes::Shape>, std::owner_less<>> _shapesToKeep;
    TabGraph::Lights::ProbeGroup _lightProbeGroup{ 1 };
    float _fixedDelta{ 0 };
    float _lightProbeDelta{ 0 };
};
};
