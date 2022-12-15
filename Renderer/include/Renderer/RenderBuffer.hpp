#pragma once

#include <Renderer/Handle.hpp>

TABGRAPH_RENDERER_HANDLE(TabGraph::Renderer);
TABGRAPH_RENDERER_HANDLE(TabGraph::Renderer::RenderBuffer);

namespace TabGraph::Renderer::RenderBuffer {
struct Info {
    uint32_t width{ 0 }, height{ 0 };
};
Handle Create(const Renderer::Handle& a_Renderer, const Info& a_Info);
}