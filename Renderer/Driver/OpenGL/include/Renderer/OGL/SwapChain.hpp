#pragma once

#include <Renderer/Handles.hpp>
#include <Renderer/Structs.hpp>
#include <Tools/WorkerThread.hpp>

#include <Renderer/OGL/RenderBuffer.hpp>

#include <array>

namespace TabGraph::Renderer::SwapChain {
struct Impl {
    Impl(
        const Renderer::Handle& a_Renderer,
        const CreateSwapChainInfo& a_Info);
    Impl(
        const Handle& a_OldSwapChain,
        const CreateSwapChainInfo& a_Info);
    ~Impl();
    void Retire();
    Tools::WorkerThread presentThread;
    std::weak_ptr<Renderer::Impl> renderer;
    std::array<RenderBuffer::Handle, 10> renderBuffers;
    uint32_t width = 0, height = 0;
    uint8_t imageCount = 0;
    bool vSync         = false;
    bool retired       = false;
#ifdef WIN32
    void* hwnd  = nullptr;
    void* hdc   = nullptr;
    void* hglrc = nullptr;
#endif
};
}
