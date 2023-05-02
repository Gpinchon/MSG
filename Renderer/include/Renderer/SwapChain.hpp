#pragma once

#include <Renderer/Handles.hpp>
#include <Renderer/Structs.hpp>

namespace TabGraph::Renderer::SwapChain {
/**
* @brief Creates a SwapChain for the specified window
*/
Handle Create(
    const Renderer::Handle& a_Renderer,
    const CreateSwapChainInfo& a_Info);

/**
* @brief Creates a SwapChain for the specified window
*/
Handle Recreate(
    const SwapChain::Handle& a_OldSwapChain,
    const CreateSwapChainInfo& a_Info);

/**
* @brief Copies the render buffer to the swapchain's back buffer and present it to the window
*/
void Present(
    const SwapChain::Handle&    a_SwapChain,
    const RenderBuffer::Handle& a_RenderBuffer);

}