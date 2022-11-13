/*
* @Author: gpinchon
* @Date:   2021-04-11 16:24:29
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-24 18:52:37
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Renderer/Light/LightRenderer.hpp>

#include <glm/glm.hpp>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph {
namespace Lights {
class SkyLight;
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

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Renderer {
class SkyLightRenderer : public LightRenderer {
public:
    SkyLightRenderer(TabGraph::Lights::SkyLight&);
    virtual void Render(const Renderer::Options&) override;
    virtual void UpdateLightProbe(const Renderer::Options&, TabGraph::Lights::Probe&) override;

protected:
    void _RenderDeferredLighting(TabGraph::Lights::SkyLight&, const Renderer::Options&);
    void _RenderShadow(TabGraph::Lights::SkyLight&, const Renderer::Options&);
    void _UpdateLUT(TabGraph::Lights::SkyLight&, const Renderer::Options&);
    std::shared_ptr<Framebuffer> _shadowBuffer;
    std::vector<glm::vec3> _SHDiffuse;
    std::shared_ptr<Textures::TextureCubemap> _reflectionLUT;
    std::shared_ptr<Shader::Program> _deferredShader;
    std::shared_ptr<Shader::Program> _probeShader;
    std::shared_ptr<Framebuffer> _diffuseLUTBuffer;
    std::shared_ptr<Shapes::Geometry> _deferredGeometry;
};
};