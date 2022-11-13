/*
* @Author: gpinchon
* @Date:   2021-04-11 20:51:53
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-11 20:52:16
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Renderer/Light/LightRenderer.hpp>

#include <glm/mat4x4.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph {
namespace Shapes {
class Geometry;
}
namespace Lights {
class PointLight;
glm::mat4 PointLightShadowViewMatrix(PointLight&);
glm::mat4 PointLightShadowProjectionMatrixInfinite(PointLight&, const Renderer::Options&);
glm::mat4 PointLightShadowProjectionMatrixFinite(PointLight&);
}
namespace Renderer {
class Framebuffer;
}
namespace Shader {
class Program;
};
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Renderer {
class PointLightRenderer : public LightRenderer {
public:
    PointLightRenderer(TabGraph::Lights::PointLight &);
    virtual void Render(const Renderer::Options&) override;
    virtual void UpdateLightProbe(const Renderer::Options&, TabGraph::Lights::Probe&) override;

protected:
    void _RenderDeferredLighting(TabGraph::Lights::PointLight&, const Renderer::Options&);
    void _RenderShadow(TabGraph::Lights::PointLight&, const Renderer::Options&);
    void _RenderShadowInfinite(TabGraph::Lights::PointLight&, const Renderer::Options&);
    void _RenderShadowFinite(TabGraph::Lights::PointLight&, const Renderer::Options&);
    std::shared_ptr<Shader::Program> _deferredShader;
    std::shared_ptr<Shader::Program> _probeShader;
    std::shared_ptr<Framebuffer> _shadowBuffer;
    std::shared_ptr<Shapes::Geometry> _deferredGeometry;
};
}


