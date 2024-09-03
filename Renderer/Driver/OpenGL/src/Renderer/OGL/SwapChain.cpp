#ifdef _WIN32
#include <Renderer/OGL/Win32/SwapChain.hpp>
#elif defined __linux__
#include <Renderer/OGL/Unix/SwapChain.hpp>
#endif

#include <Renderer/RenderBuffer.hpp>

#include <cassert>

namespace TabGraph::Renderer::SwapChain {
Handle Create(
    const Renderer::Handle& a_Renderer,
    const CreateSwapChainInfo& a_Info)
{
    return std::make_shared<Impl>(a_Renderer, a_Info);
}

Handle Recreate(
    const SwapChain::Handle& a_OldSwapChain,
    const CreateSwapChainInfo& a_Info)
{
    return std::make_shared<Impl>(a_OldSwapChain, a_Info);
}

void Present(
    const SwapChain::Handle& a_SwapChain,
    const RenderBuffer::Handle& a_RenderBuffer)
{
    a_SwapChain->Present(a_RenderBuffer);
}

void Wait(const SwapChain::Handle& a_SwapChain)
{
    a_SwapChain->Wait();
}
}
