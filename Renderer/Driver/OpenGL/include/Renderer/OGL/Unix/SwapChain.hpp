#pragma once

#include <Renderer/Handles.hpp>
#include <Renderer/Structs.hpp>

#include <Renderer/OGL/RAII/FrameBuffer.hpp>
#include <Renderer/OGL/RAII/Program.hpp>
#include <Renderer/OGL/RAII/Sampler.hpp>
#include <Renderer/OGL/RAII/Texture.hpp>
#include <Renderer/OGL/RAII/VertexArray.hpp>
#include <Renderer/OGL/RAII/Wrapper.hpp>
#include <Renderer/OGL/ShaderCompiler.hpp>

#include <array>

namespace TabGraph::Renderer::SwapChain {
struct EGLImage {
    EGLImage();
    void* image = nullptr;
};
struct Impl {
    Impl(
        const Renderer::Handle& a_Renderer,
        const CreateSwapChainInfo& a_Info);
    Impl(
        const Handle& a_OldSwapChain,
        const CreateSwapChainInfo& a_Info);
    void Present(const RenderBuffer::Handle& a_RenderBuffer);
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
