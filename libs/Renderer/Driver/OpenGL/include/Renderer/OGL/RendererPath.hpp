#pragma once

#include <memory>
#include <vector>

namespace TabGraph::Renderer {
class RenderPass;
}

namespace TabGraph::Renderer {
class Impl;
// renderPath is responsible for rendering to activeRenderBuffer
class Path {
public:
    virtual ~Path()                                 = default;
    virtual void Update(Renderer::Impl& a_Renderer) = 0;
    std::vector<std::shared_ptr<RenderPass>> renderPasses;
};
}
