#pragma once

#include <memory>
#include <vector>

namespace MSG {
class OGLRenderPass;
}

namespace MSG::Renderer {
struct RendererSettings;
}

namespace MSG::Renderer {
class Impl;
// renderPath is responsible for rendering to activeRenderBuffer
class Path {
public:
    virtual ~Path() = default;
    virtual void UpdateSettings(
        Renderer::Impl& a_Renderer,
        const RendererSettings& a_Settings)
        = 0;
    virtual void Update(Renderer::Impl& a_Renderer) = 0;
    std::vector<OGLRenderPass*> renderPasses;
};
}
