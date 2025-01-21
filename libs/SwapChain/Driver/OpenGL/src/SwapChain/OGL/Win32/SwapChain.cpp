#include <Core/Image/Pixel.hpp>
#include <Renderer/OGL/Context.hpp>
#include <Renderer/OGL/RAII/Buffer.hpp>
#include <Renderer/OGL/RAII/DebugGroup.hpp>
#include <Renderer/OGL/RAII/Program.hpp>
#include <Renderer/OGL/RAII/VertexArray.hpp>
#include <Renderer/OGL/RenderBuffer.hpp>
#include <Renderer/OGL/Renderer.hpp>

#ifdef _WIN32
#ifdef IN
#undef IN
#define NOMSG
#include <GL/wglew.h>
#include <Renderer/OGL/Win32/Context.hpp>
#include <Renderer/OGL/Win32/Platform.hpp>
#include <SwapChain/OGL/Win32/SwapChain.hpp>
#endif // IN
#elif defined(__linux__)
#include <Renderer/OGL/Unix/Context.hpp>
#endif //_WIN32

#include <GL/glew.h>

namespace MSG::SwapChain {
Renderer::Context* CreateContext(const CreateSwapChainInfo& a_Info, Renderer::Context* a_RendererCtx)
{
    bool offscreen = false;
    Renderer::CreateContextInfo info;
    info.maxPendingTasks         = a_Info.imageCount;
    info.nativeDisplayHandle     = a_Info.windowInfo.nativeDisplayHandle;
    info.setPixelFormat          = a_Info.windowInfo.setPixelFormat;
    info.pixelFormat.redBits     = Core::Pixel::GetChannelDataTypeSize(a_Info.windowInfo.pixelFormat.colorFormat, Core::Pixel::ColorChannelRed);
    info.pixelFormat.greenBits   = Core::Pixel::GetChannelDataTypeSize(a_Info.windowInfo.pixelFormat.colorFormat, Core::Pixel::ColorChannelGreen);
    info.pixelFormat.blueBits    = Core::Pixel::GetChannelDataTypeSize(a_Info.windowInfo.pixelFormat.colorFormat, Core::Pixel::ColorChannelBlue);
    info.pixelFormat.alphaBits   = Core::Pixel::GetChannelDataTypeSize(a_Info.windowInfo.pixelFormat.colorFormat, Core::Pixel::ColorChannelAlpha);
    info.pixelFormat.depthBits   = Core::Pixel::GetChannelDataTypeSize(a_Info.windowInfo.pixelFormat.depthFormat, Core::Pixel::ColorChannelDepth);
    info.pixelFormat.stencilBits = Core::Pixel::GetChannelDataTypeSize(a_Info.windowInfo.pixelFormat.stencilFormat, Core::Pixel::ColorChannelStencil);
    return reinterpret_cast<Renderer::Context*>(new Renderer::ContextT<Platform::NormalContext>(info));
}

Impl::Impl(
    const Renderer::Handle& a_Renderer,
    const CreateSwapChainInfo& a_Info)
    : context(CreateContext(a_Info, &a_Renderer->context))
    , rendererContext(a_Renderer->context)
    , imageCount(a_Info.imageCount)
    , width(a_Info.width)
    , height(a_Info.height)
    , vSync(a_Info.vSync)
{
    for (uint8_t index = 0; index < imageCount; ++index)
        images.emplace_back(Renderer::RAII::MakePtr<Renderer::RAII::Texture2D>(*context, width, height, 1, GL_RGBA8));
    Renderer::VertexAttributeDescription attribDesc {};
    attribDesc.binding           = 0;
    attribDesc.format.normalized = false;
    attribDesc.format.size       = 1;
    attribDesc.format.type       = GL_BYTE;
    Renderer::VertexBindingDescription bindingDesc {};
    bindingDesc.buffer = Renderer::RAII::MakePtr<Renderer::RAII::Buffer>(*context, 3, nullptr, 0);
    bindingDesc.index  = 0;
    bindingDesc.offset = 0;
    bindingDesc.stride = 1;
    std::vector<Renderer::VertexAttributeDescription> attribs { attribDesc };
    std::vector<Renderer::VertexBindingDescription> bindings { bindingDesc };
    presentVAO = Renderer::RAII::MakePtr<Renderer::RAII::VertexArray>(*context,
        3, attribs, bindings);
    context->PushCmd(
        [this, vSync = a_Info.vSync] {
            Platform::CtxSetSwapInterval(vSync ? 1 : 0);
            glUseProgram(*presentProgram);
            glActiveTexture(GL_TEXTURE0);
            glBindSampler(0, *presentSampler);
            glBindVertexArray(*presentVAO);
            glViewport(0, 0, width, height);
        });
    context->ExecuteCmds(true);
}

Impl::Impl(
    const Handle& a_OldSwapChain,
    const CreateSwapChainInfo& a_Info)
    : context(std::move(a_OldSwapChain->context))
    , rendererContext(a_OldSwapChain->rendererContext)
    , presentProgram(a_OldSwapChain->presentProgram)
    , presentVAO(a_OldSwapChain->presentVAO)
    , imageCount(a_Info.imageCount)
    , width(a_Info.width)
    , height(a_Info.height)
    , vSync(a_Info.vSync)
{
    uint8_t index = 0;
    if (a_OldSwapChain->width == a_Info.width && a_OldSwapChain->height == a_Info.height) {
        auto reusedImagesCount = std::min(imageCount, a_OldSwapChain->imageCount);
        images.resize(reusedImagesCount);
        while (index < reusedImagesCount) {
            images.at(index) = std::move(a_OldSwapChain->images.at(index));
            ++index;
        }
    }
    if (index < imageCount) {
        // Create the remaining render buffers
        while (index < imageCount) {
            images.emplace_back(Renderer::RAII::MakePtr<Renderer::RAII::Texture2D>(*context, width, height, 1, GL_RGBA8));
            ++index;
        }
    }
    context->PushCmd(
        [width = width, height = height, vSync = a_Info.vSync]() {
            Platform::CtxSetSwapInterval(vSync ? 1 : 0);
            glViewport(0, 0, width, height);
        });
    context->ExecuteCmds(true);
}

void Impl::Present(const Renderer::RenderBuffer::Handle& a_RenderBuffer)
{
    context->PushCmd(
        [&ctx            = context->impl,
            &rendererCtx = rendererContext,
            width        = width,
            height       = height,
            currentImage = images.at(imageIndex),
            renderBuffer = a_RenderBuffer]() {
            {
                auto copyWidth  = std::min(width, (*renderBuffer)->width);
                auto copyHeight = std::min(height, (*renderBuffer)->height);
                rendererCtx.WaitGPU();
                auto debugGroup = Renderer::RAII::DebugGroup("Present");
                wglCopyImageSubDataNV(
                    std::any_cast<HGLRC>(rendererCtx.impl->hglrc),
                    **renderBuffer, GL_TEXTURE_2D, 0, 0, 0, 0,
                    std::any_cast<HGLRC>(ctx->hglrc),
                    *currentImage, GL_TEXTURE_2D, 0, 0, 0, 0,
                    copyWidth, copyHeight, 1);
                glBindTexture(GL_TEXTURE_2D, *currentImage);
                glDrawArrays(GL_TRIANGLES, 0, 3);
            }
            Platform::CtxSwapBuffers(*ctx);
        });
    context->ExecuteCmds(vSync || context->Busy());
    imageIndex = ++imageIndex % imageCount;
}

void Impl::Wait()
{
    context->WaitWorkerThread();
}
}
