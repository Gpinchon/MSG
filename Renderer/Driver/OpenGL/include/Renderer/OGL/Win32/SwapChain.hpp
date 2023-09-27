#pragma once

#include <Renderer/Handles.hpp>
#include <Renderer/Structs.hpp>

#include <Renderer/OGL/RAII/FrameBuffer.hpp>
#include <Renderer/OGL/RAII/Sampler.hpp>
#include <Renderer/OGL/RAII/Texture.hpp>
#include <Renderer/OGL/RAII/Wrapper.hpp>
#include <Renderer/OGL/Renderer.hpp>
#include <Renderer/OGL/ShaderCompiler.hpp>

#include <array>

namespace TabGraph::Renderer::RAII {
struct ShaderCompiler;
}

namespace TabGraph::Renderer::SwapChain {
struct Impl {
    Impl(
        const Renderer::Handle& a_Renderer,
        const CreateSwapChainInfo& a_Info);
    Impl(
        const Handle& a_OldSwapChain,
        const CreateSwapChainInfo& a_Info);
    void Present(const RenderBuffer::Handle& a_RenderBuffer);
    RAII::Context context;
    RAII::Context& rendererContext;
    ShaderCompiler shaderCompiler { context };
    RAII::Wrapper<RAII::Sampler> presentSampler { RAII::MakeWrapper<RAII::Sampler>(context) };
    RAII::Wrapper<RAII::Program> presentProgram;
    RAII::Wrapper<RAII::VertexArray> presentVAO;
    std::vector<RAII::Wrapper<RAII::Texture2D>> images;
    uint8_t imageCount = 0, imageIndex = 0;
    uint32_t width = 0, height = 0;
    bool vSync = false;
};
}
