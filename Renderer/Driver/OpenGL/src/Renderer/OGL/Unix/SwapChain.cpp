#include <Renderer/OGL/RAII/Buffer.hpp>
#include <Renderer/OGL/RAII/DebugGroup.hpp>
#include <Renderer/OGL/RAII/Program.hpp>
#include <Renderer/OGL/RAII/VertexArray.hpp>
#include <Renderer/OGL/RenderBuffer.hpp>
#include <Renderer/OGL/Renderer.hpp>

#include <Renderer/ShaderLibrary.hpp>

#include <GL/glew.h>

#ifdef WIN32
#include <GL/wglew.h>
#include <Renderer/OGL/Win32/Context.hpp>
#include <Renderer/OGL/Win32/Error.hpp>
#include <Renderer/OGL/Win32/SwapChain.hpp>
#elif defined __linux__
#include <GL/glxew.h>
#include <Renderer/OGL/Unix/Context.hpp>
#include <Renderer/OGL/Unix/SwapChain.hpp>
#endif // WIN32

namespace TabGraph::Renderer::SwapChain {
Impl::Impl(
    const Renderer::Handle& a_Renderer,
    const CreateSwapChainInfo& a_Info)
    : context(std::make_unique<Context>(a_Info.windowInfo.display, a_Renderer->context.context, a_Info.windowInfo.window, a_Info.windowInfo.setPixelFormat, a_Info.windowInfo.pixelFormat, 3))
    , rendererContext(a_Renderer->context)
    , imageCount(a_Info.imageCount)
    , width(a_Info.width)
    , height(a_Info.height)
    , vSync(a_Info.vSync)
{
    auto& presentLibProgram = ShaderLibrary::GetProgram("SwapChain");
    auto shaders            = shaderCompiler.CompileProgram(presentLibProgram);
    presentProgram          = RAII::MakePtr<RAII::Program>(
        *context, shaders);
    for (uint8_t index = 0; index < imageCount; ++index)
        images.emplace_back(RAII::MakePtr<RAII::Texture2D>(*context, width, height, 1, GL_RGB8));
    VertexAttributeDescription attribDesc {};
    attribDesc.binding           = 0;
    attribDesc.format.normalized = false;
    attribDesc.format.size       = 1;
    attribDesc.format.type       = GL_BYTE;
    VertexBindingDescription bindingDesc {};
    bindingDesc.buffer = RAII::MakePtr<RAII::Buffer>(*context, 3, nullptr, 0);
    bindingDesc.index  = 0;
    bindingDesc.offset = 0;
    bindingDesc.stride = 1;
    std::vector<VertexAttributeDescription> attribs { attribDesc };
    std::vector<VertexBindingDescription> bindings { bindingDesc };
    presentVAO = RAII::MakePtr<RAII::VertexArray>(*context,
        3, attribs, bindings);
    context->PushCmd(
        [this, vSync = a_Info.vSync] {
            glXSwapIntervalEXT((Display*)context->display, context->drawableID, vSync ? 1 : 0);
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
            images.emplace_back(RAII::MakePtr<RAII::Texture2D>(*context, width, height, 1, GL_RGB8));
            ++index;
        }
    }
    context->PushCmd(
        [this, vSync = a_Info.vSync]() {
            glXSwapIntervalEXT((Display*)context->display, context->drawableID, vSync ? 1 : 0);
            glViewport(0, 0, width, height);
        });
    context->ExecuteCmds(true);
}

void Impl::Present(const RenderBuffer::Handle& a_RenderBuffer)
{
    auto waitCmds = vSync || context->Busy();
    context->PushCmd(
        [display         = context->display,
            drawable     = context->drawableID,
            width        = width,
            height       = height,
            currentImage = images.at(imageIndex),
            renderBuffer = a_RenderBuffer]() {
            auto copyWidth  = std::min(width, (*renderBuffer)->width);
            auto copyHeight = std::min(height, (*renderBuffer)->height);
            auto debugGroup = RAII::DebugGroup("Present");
            glCopyImageSubData(
                **renderBuffer, GL_TEXTURE_2D, 0, 0, 0, 0,
                *currentImage, GL_TEXTURE_2D, 0, 0, 0, 0,
                copyWidth, copyHeight, 1);
            glBindTexture(GL_TEXTURE_2D, *currentImage);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glXSwapBuffers((Display*)display, drawable);
        });
    context->ExecuteCmds(waitCmds);
    imageIndex = ++imageIndex % imageCount;
}

void Impl::Wait()
{
    context->WaitWorkerThread();
}
}
