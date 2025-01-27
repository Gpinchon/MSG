#include <MSG/OGLContext.hpp>
#include <MSG/PixelDescriptor.hpp>
#include <MSG/Renderer/OGL/RAII/Buffer.hpp>
#include <MSG/Renderer/OGL/RAII/DebugGroup.hpp>
#include <MSG/Renderer/OGL/RAII/Program.hpp>
#include <MSG/Renderer/OGL/RAII/VertexArray.hpp>
#include <MSG/Renderer/OGL/RenderBuffer.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/RenderBuffer.hpp>
#include <MSG/SwapChain/OGL/SwapChain.hpp>
#include <MSG/SwapChain/SwapChain.hpp>

#include <GL/glew.h>

#include <cassert>

namespace MSG::SwapChain {
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
    presentVAO = Renderer::RAII::MakePtr<Renderer::RAII::VertexArray>(*context, 3, attribs, bindings);
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
        while (index < imageCount) {
            images.emplace_back(Renderer::RAII::MakePtr<Renderer::RAII::Texture2D>(*context, width, height, 1, GL_RGBA8));
            ++index;
        }
    }
    context->PushCmd(
        [this, swapInterval = GetSwapInterval(a_Info.presentMode)]() {
            Platform::CtxSetSwapInterval(*context->impl, swapInterval);
            glViewport(0, 0, width, height);
        });
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

MSG::SwapChain::Handle MSG::SwapChain::Create(
    const Renderer::Handle& a_Renderer,
    const CreateSwapChainInfo& a_Info)
{
    return std::make_shared<Impl>(a_Renderer, a_Info);
}

MSG::SwapChain::Handle MSG::SwapChain::Recreate(
    const SwapChain::Handle& a_OldSwapChain,
    const CreateSwapChainInfo& a_Info)
{
    return std::make_shared<Impl>(a_OldSwapChain, a_Info);
}

void MSG::SwapChain::Present(
    const SwapChain::Handle& a_SwapChain,
    const Renderer::RenderBuffer::Handle& a_RenderBuffer)
{
    a_SwapChain->Present(a_RenderBuffer);
}

void MSG::SwapChain::Wait(const SwapChain::Handle& a_SwapChain)
{
    a_SwapChain->Wait();
}
