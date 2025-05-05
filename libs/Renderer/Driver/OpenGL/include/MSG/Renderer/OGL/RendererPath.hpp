#pragma once

#include <MSG/OGLCmdBuffer.hpp>

#include <memory>
#include <vector>

namespace MSG::Renderer {
class Impl;
struct RendererSettings;
}

namespace MSG::Renderer {
// renderPath is responsible for rendering to activeRenderBuffer
class Path {
public:
    Path(Renderer::Impl& a_Renderer);
    virtual ~Path() = default;
    virtual void UpdateSettings(
        Renderer::Impl& a_Renderer,
        const RendererSettings& a_Settings)
        = 0;
    virtual void UpdateRenderBuffers(
        Renderer::Impl& a_Renderer)
        = 0;
    virtual void Update(Renderer::Impl& a_Renderer) = 0;
    OGLCmdBuffer cmdBuffer;
};
}
