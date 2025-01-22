#pragma once

#include <MSG/Renderer/Handles.hpp>
#include <MSG/Renderer/Structs.hpp>

#include <MSG/Renderer/OGL/RAII/FrameBuffer.hpp>
#include <MSG/Renderer/OGL/RAII/Program.hpp>
#include <MSG/Renderer/OGL/RAII/Sampler.hpp>
#include <MSG/Renderer/OGL/RAII/Texture.hpp>
#include <MSG/Renderer/OGL/RAII/VertexArray.hpp>
#include <MSG/Renderer/OGL/RAII/Wrapper.hpp>
#include <MSG/Renderer/OGL/ShaderCompiler.hpp>

#include <array>

namespace MSG::Renderer::SwapChain {
struct Impl {
    Impl(
        const Renderer::Handle& a_Renderer,
        const CreateSwapChainInfo& a_Info);
    Impl(
        const Handle& a_OldSwapChain,
        const CreateSwapChainInfo& a_Info);
    void Present(const RenderBuffer::Handle& a_RenderBuffer);
    void Wait();
    std::unique_ptr<Context> context;
    Context& rendererContext;
    ShaderCompiler shaderCompiler { *context };
    std::shared_ptr<RAII::Sampler> presentSampler { RAII::MakePtr<RAII::Sampler>(*context) };
    std::shared_ptr<RAII::Program> presentProgram;
    std::shared_ptr<RAII::VertexArray> presentVAO;
    std::vector<std::shared_ptr<RAII::Texture2D>> images;
    uint8_t imageCount = 0;
    uint8_t imageIndex = 0;
    uint32_t width     = 0;
    uint32_t height    = 0;
    bool vSync         = false;
};
}
