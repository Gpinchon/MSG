#pragma once

#include <MSG/OGLSampler.hpp>
#include <MSG/OGLVertexArray.hpp>
#include <MSG/Renderer/Handles.hpp>
#include <MSG/Renderer/OGL/ShaderCompiler.hpp>
#include <MSG/SwapChain/Handles.hpp>
#include <MSG/SwapChain/Structs.hpp>

#include <array>

namespace MSG {
class OGLContext;
class OGLTexture2D;
class OGLProgram;
}

namespace MSG::SwapChain {
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
    std::unique_ptr<OGLContext> context;
    OGLContext& rendererContext;
    Renderer::ShaderCompiler shaderCompiler    = { *context };
    std::shared_ptr<OGLSampler> presentSampler = std::make_shared<OGLSampler>(*context);
    std::shared_ptr<OGLProgram> presentProgram = shaderCompiler.CompileProgram("SwapChain");
    std::shared_ptr<OGLVertexArray> presentVAO;
    std::vector<std::shared_ptr<OGLTexture2D>> images;
    uint8_t imageCount = 0;
    uint8_t imageIndex = 0;
    uint32_t width     = 0;
    uint32_t height    = 0;
};
}
