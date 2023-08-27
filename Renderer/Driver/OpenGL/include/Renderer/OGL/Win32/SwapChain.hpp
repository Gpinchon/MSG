#pragma once

#include <Renderer/Handles.hpp>
#include <Renderer/Structs.hpp>

#include <Renderer/OGL/RAII/FrameBuffer.hpp>
#include <Renderer/OGL/RAII/Sampler.hpp>
#include <Renderer/OGL/RAII/Texture.hpp>
#include <Renderer/OGL/RAII/Wrapper.hpp>
#include <Renderer/OGL/Renderer.hpp>

#include <array>

namespace TabGraph::Renderer::SwapChain {
struct Image {
    Image(
        RAII::Context& context,
        const uint32_t a_Width,
        const uint32_t a_Height);
    void Blit();
    RAII::Wrapper<RAII::FrameBuffer> frameBuffer;
    RAII::Wrapper<RAII::Texture2D> texture;
};

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
    RAII::Wrapper<RAII::Sampler> sampler { &context };
    RAII::Wrapper<RAII::FrameBuffer> frameBuffer { &context };
    std::vector<Image> images;
    uint8_t imageCount = 0, imageIndex = 0;
    uint32_t width = 0, height = 0;
    bool vSync = false;
};
}
