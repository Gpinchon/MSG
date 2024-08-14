#pragma once

namespace TabGraph::Renderer {
class Impl;
// renderPath is responsible for rendering to activeRenderBuffer
class Path {
public:
    virtual ~Path()                                 = default;
    virtual void Update(Renderer::Impl& a_Renderer) = 0;
    virtual void Execute()                          = 0;
};
}