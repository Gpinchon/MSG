#include <MSG/OGLBuffer.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLDebugGroup.hpp>
#include <MSG/OGLProgram.hpp>
#include <MSG/OGLTexture2D.hpp>
#include <MSG/OGLVertexArray.hpp>
#include <MSG/PixelDescriptor.hpp>
#include <MSG/Renderer/OGL/RenderBuffer.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/RenderBuffer.hpp>
#include <MSG/SwapChain/OGL/SwapChain.hpp>
#include <MSG/SwapChain/SwapChain.hpp>

#include <GL/glew.h>

#include <cassert>

namespace Msg::SwapChain {
int8_t GetSwapInterval(const PresentMode& a_PresentMode)
{
    int8_t interval = 0;
    switch (a_PresentMode) {
    case PresentMode::Immediate:
        interval = 0;
        break;
    case PresentMode::FIFO:
    case PresentMode::MailBox:
        interval = 1;
        break;
    case PresentMode::FIFORelaxed:
        interval = -1;
        break;
    default:
        break;
    }
    return interval;
}

uint8_t GetMaxPendingTasks(const PresentMode& a_PresentMode, const uint8_t& a_ImageCount)
{
    int8_t maxPendingTasks = 0;
    switch (a_PresentMode) {
    case PresentMode::Immediate:
        maxPendingTasks = 0;
        break;
    case PresentMode::MailBox:
        maxPendingTasks = 1;
        break;
    case PresentMode::FIFO:
    case PresentMode::FIFORelaxed:
        maxPendingTasks = a_ImageCount;
        break;
    default:
        break;
    }
    return maxPendingTasks;
}

std::unique_ptr<OGLContext> CreateContext(const Renderer::Handle& a_Renderer, const CreateSwapChainInfo& a_Info)
{
    OGLContextCreateInfo info;
    info.sharedContext       = &a_Renderer->context;
    info.maxPendingTasks     = GetMaxPendingTasks(a_Info.presentMode, a_Info.imageCount);
    info.nativeDisplayHandle = a_Info.windowInfo.nativeDisplayHandle;
    info.nativeWindowHandle  = a_Info.windowInfo.nativeWindowHandle;
    info.setPixelFormat      = a_Info.windowInfo.setPixelFormat;
    return std::make_unique<OGLContext>(CreateNormalOGLContext(info));
}

Impl::Impl(
    const Renderer::Handle& a_Renderer,
    const CreateSwapChainInfo& a_Info)
    : context(CreateContext(a_Renderer, a_Info))
    , rendererContext(a_Renderer->context)
    , imageCount(a_Info.imageCount)
    , width(a_Info.width)
    , height(a_Info.height)
{
    OGLTexture2DInfo textureInfo { .width = width, .height = height, .levels = 1, .sizedFormat = GL_RGBA8 };
    for (uint8_t index = 0; index < imageCount; ++index)
        images.emplace_back(std::make_shared<OGLTexture2D>(*context, textureInfo));
    OGLVertexAttributeDescription attribDesc {};
    attribDesc.binding           = 0;
    attribDesc.format.normalized = false;
    attribDesc.format.size       = 1;
    attribDesc.format.type       = GL_BYTE;
    OGLVertexBindingDescription bindingDesc {};
    bindingDesc.buffer = std::make_shared<OGLBuffer>(*context, 3, nullptr, 0);
    bindingDesc.index  = 0;
    bindingDesc.offset = 0;
    bindingDesc.stride = 1;
    std::vector<OGLVertexAttributeDescription> attribs { attribDesc };
    std::vector<OGLVertexBindingDescription> bindings { bindingDesc };
    presentVAO = std::make_shared<OGLVertexArray>(*context, 3, attribs, bindings);
    context->PushCmd(
        [this, swapInterval = GetSwapInterval(a_Info.presentMode)] {
            Platform::CtxSetSwapInterval(*context->impl, swapInterval);
            glUseProgram(*presentProgram);
            glActiveTexture(GL_TEXTURE0);
            glBindSampler(0, *presentSampler);
            glBindVertexArray(*presentVAO);
            glViewport(0, 0, width, height);
        });
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
        OGLTexture2DInfo textureInfo { .width = width, .height = height, .levels = 1, .sizedFormat = GL_RGBA8 };
        while (index < imageCount) {
            images.emplace_back(std::make_shared<OGLTexture2D>(*context, textureInfo));
            ++index;
        }
    }
    context->PushCmd(
        [&platformCtx = *context->impl, width = width, height = height, swapInterval = GetSwapInterval(a_Info.presentMode)]() {
            Platform::CtxSetSwapInterval(platformCtx, swapInterval);
            glViewport(0, 0, width, height);
        });
}

void Impl::Present(const RenderBuffer::Handle& a_RenderBuffer)
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
                auto debugGroup = OGLDebugGroup("SwapChain::Present");
                glCopyImageSubData(
                    **renderBuffer, GL_TEXTURE_2D, 0, 0, 0, 0,
                    *currentImage, GL_TEXTURE_2D, 0, 0, 0, 0,
                    copyWidth, copyHeight, 1);
                glBindTexture(GL_TEXTURE_2D, *currentImage);
                glDrawArrays(GL_TRIANGLES, 0, 3);
            }
            Platform::CtxSwapBuffers(*ctx);
        },
        context->Busy());
    imageIndex = ++imageIndex % imageCount;
}

void Impl::Wait()
{
    context->WaitWorkerThread();
}
}

Msg::SwapChain::Handle Msg::SwapChain::Create(
    const Renderer::Handle& a_Renderer,
    const CreateSwapChainInfo& a_Info)
{
    return std::make_shared<Impl>(a_Renderer, a_Info);
}

Msg::SwapChain::Handle Msg::SwapChain::Recreate(
    const SwapChain::Handle& a_OldSwapChain,
    const CreateSwapChainInfo& a_Info)
{
    return std::make_shared<Impl>(a_OldSwapChain, a_Info);
}

void Msg::SwapChain::Present(
    const SwapChain::Handle& a_SwapChain,
    const RenderBuffer::Handle& a_RenderBuffer)
{
    a_SwapChain->Present(a_RenderBuffer);
}

void Msg::SwapChain::Wait(const SwapChain::Handle& a_SwapChain)
{
    a_SwapChain->Wait();
}
