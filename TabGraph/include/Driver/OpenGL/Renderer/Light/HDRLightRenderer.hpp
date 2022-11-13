/*
* @Author: gpinchon
* @Date:   2021-04-11 14:45:14
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-11 15:39:15
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Renderer/Light/LightRenderer.hpp>

#include <glm/glm.hpp>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph {
namespace Shader {
class Program;
}
namespace Shapes {
class Geometry;
}
namespace Lights {
class HDRLight;
}
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Renderer {
class HDRLightRenderer : public LightRenderer {
public:
    HDRLightRenderer(TabGraph::Lights::HDRLight&);
    virtual void Render(const Options&) override;
    virtual void UpdateLightProbe(const Renderer::Options&, TabGraph::Lights::Probe&) override;

protected:
    void _RenderDeferredLighting(TabGraph::Lights::HDRLight&, const Renderer::Options&);
    void _Update(TabGraph::Lights::HDRLight&);
    std::vector<glm::vec3> _SHDiffuse;
    std::shared_ptr<Shader::Program> _deferredShader;
    std::shared_ptr<Shader::Program> _probeShader;
    std::shared_ptr<Shapes::Geometry> _deferredGeometry;
};
};