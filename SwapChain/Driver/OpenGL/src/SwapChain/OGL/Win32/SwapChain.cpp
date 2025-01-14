#include <Renderer/OGL/RAII/Buffer.hpp>
#include <Renderer/OGL/RAII/DebugGroup.hpp>
#include <Renderer/OGL/RAII/Program.hpp>
#include <Renderer/OGL/RAII/VertexArray.hpp>
#include <Renderer/OGL/RenderBuffer.hpp>
#include <Renderer/OGL/Win32/Context.hpp>
#include <Renderer/OGL/Win32/Error.hpp>
#include <SwapChain/OGL/Win32/SwapChain.hpp>

// Because Windows...
#ifdef IN
#undef IN
#endif // IN

#include <GL/glew.h>
#include <GL/wglew.h>

namespace TabGraph::SwapChain {
auto CreateContext(const CreateSwapChainInfo& a_Info)
{
    bool offscreen = false;
    return new Renderer::Context(
        std::any_cast<HWND>(a_Info.windowInfo.nativeWindowHandle),
        a_Info.windowInfo.setPixelFormat,
        a_Info.windowInfo.pixelFormat,
        offscreen,
        a_Info.imageCount);
}
Impl::Impl(
    const Renderer::Handle& a_Renderer,
    const CreateSwapChainInfo& a_Info)
    : context(CreateContext(a_Info))
    , rendererContext(a_Renderer->context)
    , width(a_Info.width)
    , height(a_Info.height)
    , imageCount(a_Info.imageCount)
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
            WIN32_CHECK_ERROR(wglSwapIntervalEXT(vSync ? 1 : 0));
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
    , width(a_Info.width)
    , height(a_Info.height)
    , imageCount(a_Info.imageCount)
    , vSync(a_Info.vSync)
    , presentProgram(a_OldSwapChain->presentProgram)
    , presentVAO(a_OldSwapChain->presentVAO)
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
            WIN32_CHECK_ERROR(wglSwapIntervalEXT(vSync ? 1 : 0));
            glViewport(0, 0, width, height);
        });
    context->ExecuteCmds(true);
}

void Impl::Present(const Renderer::RenderBuffer::Handle& a_RenderBuffer)
{
    auto waitCmds = vSync || context->Busy();
    context->PushCmd(
        [hdc                 = context->hdc,
            hglrc            = context->hglrc,
            width            = width,
            height           = height,
            currentImage     = images.at(imageIndex),
            renderBuffer     = a_RenderBuffer,
            &rendererContext = rendererContext]() {
            {
                auto copyWidth  = std::min(width, (*renderBuffer)->width);
                auto copyHeight = std::min(height, (*renderBuffer)->height);
                rendererContext.Wait();
                auto debugGroup = Renderer::RAII::DebugGroup("Present");
                wglCopyImageSubDataNV(
                    HGLRC(rendererContext.hglrc),
                    **renderBuffer, GL_TEXTURE_2D,
                    0, 0, 0, 0,
                    HGLRC(hglrc),
                    *currentImage, GL_TEXTURE_2D,
                    0, 0, 0, 0,
                    copyWidth, copyHeight, 1);
                glBindTexture(GL_TEXTURE_2D, *currentImage);
                glDrawArrays(GL_TRIANGLES, 0, 3);
            }
            WIN32_CHECK_ERROR(SwapBuffers(HDC(hdc)));
        });
    context->ExecuteCmds(waitCmds);
    imageIndex = ++imageIndex % imageCount;
}

void Impl::Wait()
{
    context->WaitWorkerThread();
}
}
