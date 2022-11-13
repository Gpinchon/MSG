/*
* @Author: gpinchon
* @Date:   2021-04-10 15:28:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-10 15:28:08
*/
#pragma once

#include <Renderer/Light/LightRenderer.hpp>
#include <Cameras/Camera.hpp>

#include <glm/glm.hpp>

namespace TabGraph {
namespace Lights {
class DirectionalLight;
}
namespace Shapes {
class Geometry;
}
namespace Textures {
class TextureCubemap;
}
namespace Renderer {
class Framebuffer;
}
namespace Shader {
class Program;
}
}

namespace TabGraph::Renderer {
struct Options;
class DirectionalLightRenderer : public LightRenderer {
public:
    DirectionalLightRenderer(TabGraph::Lights::DirectionalLight&);
    virtual void Render(const Renderer::Options&) override;
    virtual void UpdateLightProbe(const Renderer::Options& options, TabGraph::Lights::Probe&) override;

protected:
    void _RenderDeferredLighting(TabGraph::Lights::DirectionalLight&, const Renderer::Options&);
    void _RenderShadow(TabGraph::Lights::DirectionalLight&, const Renderer::Options&);
    void _RenderShadowInfinite(TabGraph::Lights::DirectionalLight&, const Renderer::Options&);
    void _RenderShadowFinite(TabGraph::Lights::DirectionalLight&, const Renderer::Options&);
    std::shared_ptr<Shader::Program> _deferredShader;
    std::shared_ptr<Shader::Program> _probeShader;
    std::shared_ptr<Framebuffer> _shadowBuffer;
    std::shared_ptr<Shapes::Geometry> _deferredGeometry;
    std::vector<glm::vec3> _SHDiffuse;
};
glm::mat4 DirectionalLightShadowViewMatrix(TabGraph::Lights::DirectionalLight&);
TabGraph::Cameras::Projection DirectionalLightShadowProjectionInfinite(TabGraph::Lights::DirectionalLight&, const TabGraph::Renderer::Options&);
TabGraph::Cameras::Projection DirectionalLightShadowProjectionFinite(TabGraph::Lights::DirectionalLight&);
};
