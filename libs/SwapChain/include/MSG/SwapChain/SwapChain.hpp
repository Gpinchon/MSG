#pragma once

#include <MSG/Renderer/Handles.hpp>
#include <MSG/SwapChain/Handles.hpp>
#include <MSG/SwapChain/Structs.hpp>

namespace Msg::SwapChain {
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
 * The RenderBuffer should at least be the size of the SwapChain
 */
void Present(
    const Handle& a_SwapChain,
    const Renderer::RenderBuffer::Handle& a_RenderBuffer);

/**
 * @brief Waits for the swapchain to finish presentation
 */
void Wait(const SwapChain::Handle& a_SwapChain);
}
