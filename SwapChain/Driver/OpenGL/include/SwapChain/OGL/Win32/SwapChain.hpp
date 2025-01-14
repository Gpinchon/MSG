#pragma once

#include <Renderer/Handles.hpp>
#include <SwapChain/Handles.hpp>
#include <SwapChain/Structs.hpp>

#include <Renderer/OGL/RAII/FrameBuffer.hpp>
#include <Renderer/OGL/RAII/Sampler.hpp>
#include <Renderer/OGL/RAII/Texture.hpp>
#include <Renderer/OGL/RAII/Wrapper.hpp>
#include <Renderer/OGL/Renderer.hpp>
#include <Renderer/OGL/ShaderCompiler.hpp>

#include <array>

namespace TabGraph::SwapChain {
class Impl {
public:
    Impl(
        const Renderer::Handle& a_Renderer,
        const CreateSwapChainInfo& a_Info);
    Impl(
        const SwapChain::Handle& a_OldSwapChain,
        const CreateSwapChainInfo& a_Info);
    void Present(const Renderer::RenderBuffer::Handle& a_RenderBuffer);
    void Wait();
    std::unique_ptr<Renderer::Context> context;
    Renderer::Context& rendererContext;
    Renderer::ShaderCompiler shaderCompiler                 = *context;
    std::shared_ptr<Renderer::RAII::Sampler> presentSampler = Renderer::RAII::MakePtr<Renderer::RAII::Sampler>(*context);
    std::shared_ptr<Renderer::RAII::Program> presentProgram = shaderCompiler.CompileProgram("SwapChain");
    std::shared_ptr<Renderer::RAII::VertexArray> presentVAO;
    std::vector<std::shared_ptr<Renderer::RAII::Texture2D>> images;
    uint8_t imageCount = 0, imageIndex = 0;
    uint32_t width = 0, height = 0;
    bool vSync = false;
};
}
