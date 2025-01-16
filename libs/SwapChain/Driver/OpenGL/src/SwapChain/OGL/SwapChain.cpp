#ifdef _WIN32
#include <SwapChain/OGL/Win32/SwapChain.hpp>
#elif defined __linux__
#include <SwapChain/OGL/Unix/SwapChain.hpp>
#endif

#include <Renderer/RenderBuffer.hpp>
#include <SwapChain/SwapChain.hpp>

#include <cassert>

TabGraph::SwapChain::Handle TabGraph::SwapChain::Create(
    const Renderer::Handle& a_Renderer,
    const CreateSwapChainInfo& a_Info)
{
    return std::make_shared<Impl>(a_Renderer, a_Info);
}

TabGraph::SwapChain::Handle TabGraph::SwapChain::Recreate(
    const SwapChain::Handle& a_OldSwapChain,
    const CreateSwapChainInfo& a_Info)
{
    return std::make_shared<Impl>(a_OldSwapChain, a_Info);
}

void TabGraph::SwapChain::Present(
    const SwapChain::Handle& a_SwapChain,
    const Renderer::RenderBuffer::Handle& a_RenderBuffer)
{
    a_SwapChain->Present(a_RenderBuffer);
}

void TabGraph::SwapChain::Wait(const SwapChain::Handle& a_SwapChain)
{
    a_SwapChain->Wait();
}
