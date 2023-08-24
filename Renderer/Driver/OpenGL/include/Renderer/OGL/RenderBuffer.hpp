#pragma once

#include <Renderer/Handles.hpp>

namespace TabGraph::Renderer::RenderBuffer {
struct Impl {
    Impl(
        const Renderer::Handle& a_Renderer,
        const CreateRenderBufferInfo& a_Info);
    ~Impl();
    std::weak_ptr<Renderer::Impl> renderer;
    unsigned handle;
};
}
