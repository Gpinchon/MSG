#include <Renderer/OGL/Win32/Context.hpp>
#include <Renderer/OGL/Win32/Error.hpp>
#include <Renderer/OGL/Win32/SwapChain.hpp>

#include <Renderer/OGL/RenderBuffer.hpp>
#include <Renderer/OGL/Renderer.hpp>

#include <GL/glew.h>
#include <GL/wglew.h>

namespace TabGraph::Renderer::SwapChain {
Impl::Impl(
    RAII::Context& a_RendererContext,
    const CreateSwapChainInfo& a_Info)
    : context(a_Info.hwnd)
    , rendererContext(a_RendererContext)
    , width(a_Info.width)
    , height(a_Info.height)
    , imageCount(a_Info.imageCount)
    , vSync(a_Info.vSync)
{
    CreateRenderBufferInfo renderBufferInfo;
    renderBufferInfo.width  = width;
    renderBufferInfo.height = height;
    for (uint8_t index = 0; index < imageCount; ++index) {
        renderBuffers.at(index).reset(&context, width, height, 1, GL_RGB8);
    }
    context.PushRenderCmd([a_Info] {
        if (a_Info.vSync) {
            WIN32_CHECK_ERROR(wglSwapIntervalEXT(1));
        } else {
            WIN32_CHECK_ERROR(wglSwapIntervalEXT(0));
        }
    },
        false);
}

Impl::Impl(
    const Handle& a_OldSwapChain,
    const CreateSwapChainInfo& a_Info)
    : context(std::move(a_OldSwapChain->context))
    , rendererContext(a_OldSwapChain->rendererContext)
    , width(a_Info.width)
    , height(a_Info.height)
    , imageCount(a_Info.imageCount)
    , vSync(a_Info.vSync)
{
    uint8_t index = 0;
    if (a_OldSwapChain->width == a_Info.width && a_OldSwapChain->height == a_Info.height) {
        while (index < a_OldSwapChain->imageCount && index < imageCount) {
            renderBuffers.at(index) = std::move(a_OldSwapChain->renderBuffers.at(index));
            ++index;
        }
    }
    if (index < imageCount) {
        // Create the remaining render buffers
        while (index < imageCount) {
            renderBuffers.at(index).reset(&context, width, height, 1, GL_RGB8);
            ++index;
        }
    }
}

void Impl::Present(const RenderBuffer::Handle& a_RenderBuffer)
{
    context.PushRenderCmd(
        [this, &renderBuffer = *a_RenderBuffer] {
            rendererContext.PushRenderCmd(
                [this, &renderBuffer] {
                    wglCopyImageSubDataNV(
                        // HGLRC(rendererContext.hglrc),
                        HGLRC(nullptr),
                        renderBuffer->handle, GL_TEXTURE_2D,
                        0, 0, 0, 0,
                        HGLRC(context.hglrc),
                        renderBuffers.at(imageIndex)->handle, GL_TEXTURE_2D,
                        0, 0, 0, 0,
                        std::min(width, renderBuffer->width),
                        std::min(height, renderBuffer->height), 1);
                },
                true);
            glNamedFramebufferTexture(
                *frameBuffer,
                GL_COLOR_ATTACHMENT0,
                *renderBuffers.at(imageIndex),
                0);
            glBlitNamedFramebuffer(
                *frameBuffer,
                0, // default FB
                0, 0, width, height,
                0, 0, width, height,
                GL_COLOR_BUFFER_BIT, GL_LINEAR);
            glFinish();
            SwapBuffers(HDC(context.hdc));
        },
        true);
    imageIndex = ++imageIndex % imageCount;
}
}
