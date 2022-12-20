#pragma once

#include <Renderer/Handle.hpp>

#include <chrono>

TABGRAPH_RENDERER_HANDLE(TabGraph::Renderer);
TABGRAPH_RENDERER_HANDLE(TabGraph::Renderer::SwapChain);
TABGRAPH_RENDERER_HANDLE(TabGraph::Renderer::RenderBuffer);

namespace TabGraph::Renderer::SwapChain {
struct Info {
    bool vSync{ true };
    uint32_t width{ 0 }, height{ 0 };
    uint32_t imageCount{ 1 };
#ifdef WIN32
    void* hwnd{ nullptr };
#endif
};
/**
* @brief Creates a SwapChain for the specified window
*/
Handle Create(
    const Renderer::Handle& a_Renderer,
    const Info& a_Info);

/**
* @brief Creates a SwapChain for the specified window
*/
Handle Recreate(
    const SwapChain::Handle& a_OldSwapChain,
    const Info& a_Info);

/**
* @brief Presents the swapchain to the window
*/
void Present(
    const SwapChain::Handle& a_SwapChain);

/**
* @return the current back buffer of the SwapChain
*/
RenderBuffer::Handle GetNextBuffer(const Handle& a_SwapChain);

}