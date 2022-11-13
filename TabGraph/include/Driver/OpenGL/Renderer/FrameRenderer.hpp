/*
* @Author: gpinchon
* @Date:   2021-06-19 09:51:36
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:12:52
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Driver/OpenGL/Renderer/Context.hpp>
#include <Events/Signal.hpp>
#include <Renderer/FrameRenderer.hpp>

#include <glm/fwd.hpp>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph {
namespace Textures {
class Texture2D;
}
namespace Renderer {
class Framebuffer;
}
namespace Shapes {
class Geometry;
}
namespace Shader {
class Program;
}
}

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Renderer {
struct FrameRenderer::Impl {
    Impl(std::weak_ptr<Core::Window> window, FrameRenderer& renderer);
    const OpenGL::Context& GetContext() const;
    const uint32_t GetFrameNumber() const;
    const std::shared_ptr<Shapes::Geometry> GetDisplayQuad() const;
    const std::shared_ptr<Textures::Texture2D> GetDefaultBRDFLUT() const;
    const std::shared_ptr<Core::Window> GetWindow() const;
    void RenderFrame(std::shared_ptr<Nodes::Scene> scene);

    void SetViewPort(const glm::ivec2& min, const glm::ivec2& max);
    void SetViewPort(const glm::ivec2& size);
    void SetSwapInterval(SwapInterval swapInterval);
    SwapInterval GetSwapInterval() const;

    std::shared_ptr<Framebuffer> DeferredGeometryBuffer();
    std::shared_ptr<Framebuffer> DeferredLightingBuffer();
    std::shared_ptr<Framebuffer> ForwardTransparentRenderBuffer();
    std::shared_ptr<Framebuffer> OpaqueRenderBuffer();
    std::shared_ptr<Framebuffer> FinalRenderBuffer();
    std::shared_ptr<Framebuffer> PreviousRenderBuffer();

private:
    OpenGL::Context _context;
    std::shared_ptr<Framebuffer> _deferredLightingBuffer;
    std::shared_ptr<Framebuffer> _deferredRenderBuffer;
    std::shared_ptr<Framebuffer> _forwardTransparentRenderBuffer;
    std::shared_ptr<Framebuffer> _opaqueRenderBuffer;
    std::shared_ptr<Framebuffer> _finalRenderBuffer;
    std::shared_ptr<Framebuffer> _previousRenderBuffer;

    void _RenderFrame(std::shared_ptr<Nodes::Scene> scene);
    void _OpaquePass(std::shared_ptr<Nodes::Scene> scene);
    void _LightPass(std::shared_ptr<Nodes::Scene> scene);
    void _TransparentPass(std::shared_ptr<Nodes::Scene> scene);
    void _HZBPass();
    void _SSRPass();
    void _SSAOPass();
    void _DeferredMaterialPass();
    void _CompositingPass();

    std::shared_ptr<Framebuffer> _ssrBuffer;
    std::shared_ptr<Shader::Program> _ssrShader;
    std::shared_ptr<Shader::Program> _ssrStencilShader;
    std::shared_ptr<Shader::Program> _ssrApplyShader;

    std::shared_ptr<Framebuffer> _ssaoBuffer;
    std::shared_ptr<Shader::Program> _ssaoShader;

    std::shared_ptr<Framebuffer> _hzbBuffer;
    std::shared_ptr<Shader::Program> _hzbShader;

    std::shared_ptr<Shader::Program> _compositingShader;

    std::shared_ptr<Shader::Program> _deferredMaterialShader;

    std::shared_ptr<Textures::Texture2D> _defaultBRDF;
    std::shared_ptr<Shapes::Geometry> _displayQuad;

    std::weak_ptr<Core::Window> _window;

    uint32_t _frameNbr { 0 };
    FrameRenderer& _frameRenderer;
    double _lastTicks { 0 };
    double _deltaTime { 0 };
};
};