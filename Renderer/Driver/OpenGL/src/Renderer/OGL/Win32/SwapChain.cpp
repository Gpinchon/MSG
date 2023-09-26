#include <Renderer/OGL/RAII/DebugGroup.hpp>
#include <Renderer/OGL/RenderBuffer.hpp>
#include <Renderer/OGL/Win32/Context.hpp>
#include <Renderer/OGL/Win32/Error.hpp>
#include <Renderer/OGL/Win32/SwapChain.hpp>

#include <GL/glew.h>
#include <GL/wglew.h>

namespace TabGraph::Renderer::SwapChain {
Impl::Impl(
    RAII::Context& a_RendererContext,
    const CreateSwapChainInfo& a_Info)
    : context(a_Info.hwnd, false)
    , rendererContext(a_RendererContext)
    , width(a_Info.width)
    , height(a_Info.height)
    , imageCount(a_Info.imageCount)
    , vSync(a_Info.vSync)
{
    for (uint8_t index = 0; index < imageCount; ++index)
        images.emplace_back(context, width, height);
    context.PushRenderCmd([this, a_Info] {
        if (a_Info.vSync) {
            WIN32_CHECK_ERROR(wglSwapIntervalEXT(1));
        } else {
            WIN32_CHECK_ERROR(wglSwapIntervalEXT(0));
        }
    });
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
            images.at(index) = std::move(a_OldSwapChain->images.at(index));
            ++index;
        }
    }
    if (index < imageCount) {
        // Create the remaining render buffers
        while (index < imageCount) {
            images.emplace_back(context, width, height);
            ++index;
        }
    }
    context.PushRenderCmd([this, a_Info] {
        if (a_Info.vSync) {
            WIN32_CHECK_ERROR(wglSwapIntervalEXT(1));
        } else {
            WIN32_CHECK_ERROR(wglSwapIntervalEXT(0));
        }
    });
}

void Impl::Present(const RenderBuffer::Handle& a_RenderBuffer)
{
    context.PushRenderCmd(
        [this, &renderBuffer = *a_RenderBuffer] {
            {
                auto copyWidth     = std::min(width, renderBuffer->width);
                auto copyHeight    = std::min(height, renderBuffer->height);
                auto& currentImage = images.at(imageIndex);
                rendererContext.Wait();
                auto debugGroup = RAII::DebugGroup("Present");
                wglCopyImageSubDataNV(
                    HGLRC(rendererContext.hglrc),
                    *renderBuffer, GL_TEXTURE_2D,
                    0, 0, 0, 0,
                    HGLRC(context.hglrc),
                    *currentImage.texture, GL_TEXTURE_2D,
                    0, 0, 0, 0,
                    copyWidth, copyHeight, 1);
                currentImage.Blit();
                glFlush();
            }
            WIN32_CHECK_ERROR(SwapBuffers(HDC(context.hdc)));
        });
    context.ExecuteResourceCreationCmds(true);
    imageIndex = ++imageIndex % imageCount;
}

Image::Image(RAII::Context& a_Context, const uint32_t a_Width, const uint32_t a_Height)
    : frameBuffer(RAII::MakeWrapper<RAII::FrameBuffer>(a_Context, a_Width, a_Height))
    , texture(RAII::MakeWrapper<RAII::Texture2D>(a_Context, a_Width, a_Height, 1, GL_RGB8))
{
    a_Context.PushResourceCreationCmd(
        [this] {
            glNamedFramebufferTexture(
                *frameBuffer,
                GL_COLOR_ATTACHMENT0, *texture, 0);
        });
    a_Context.ExecuteResourceCreationCmds(true);
}

void Image::Blit()
{
    glBlitNamedFramebuffer(
        *frameBuffer, 0,
        0, 0, texture->width, texture->height,
        0, 0, texture->width, texture->height,
        GL_COLOR_BUFFER_BIT, GL_LINEAR);
}
}
