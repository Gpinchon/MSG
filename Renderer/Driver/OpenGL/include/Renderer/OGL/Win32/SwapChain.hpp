#pragma once

#include <Renderer/Handles.hpp>
#include <Renderer/Structs.hpp>

#include <Renderer/OGL/RAII/Sampler.hpp>
#include <Renderer/OGL/RAII/Texture.hpp>
#include <Renderer/OGL/RAII/Wrapper.hpp>
#include <Renderer/OGL/Renderer.hpp>

#include <array>

namespace TabGraph::Renderer::SwapChain {

struct Impl {
    Impl(
        RAII::Context& a_RendererContext,
        const CreateSwapChainInfo& a_Info);
    Impl(
        const Handle& a_OldSwapChain,
        const CreateSwapChainInfo& a_Info);
    void Present(const RenderBuffer::Handle& a_RenderBuffer);
    RAII::Context context;
    RAII::Context& rendererContext;
    std::array<RAII::Wrapper<RAII::Texture2D>, 10> renderBuffers;
    RAII::Wrapper<RAII::Sampler> sampler { &context };
    uint32_t width = 0, height = 0;
    uint8_t imageCount = 0, imageIndex = 0;
    bool vSync = false;
};
}
