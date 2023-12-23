#include <Renderer/OGL/RAII/Buffer.hpp>
#include <Renderer/OGL/RAII/DebugGroup.hpp>
#include <Renderer/OGL/RAII/Program.hpp>
#include <Renderer/OGL/RAII/VertexArray.hpp>
#include <Renderer/OGL/RenderBuffer.hpp>
#include <Renderer/OGL/Renderer.hpp>

#include <GL/glew.h>

#ifdef WIN32
#include <GL/wglew.h>
#include <Renderer/OGL/Win32/Context.hpp>
#include <Renderer/OGL/Win32/Error.hpp>
#include <Renderer/OGL/Win32/SwapChain.hpp>
#elifdef __linux__
#include <GL/glxew.h>
#include <Renderer/OGL/Unix/Context.hpp>
#include <Renderer/OGL/Unix/SwapChain.hpp>
#endif // WIN32

namespace TabGraph::Renderer::SwapChain {
Impl::Impl(
    const Renderer::Handle& a_Renderer,
    const CreateSwapChainInfo& a_Info)
    : context(std::make_unique<Context>(a_Info.windowInfo.display, a_Info.windowInfo.window, a_Info.windowInfo.setPixelFormat, a_Info.windowInfo.pixelFormat, 3))
    , rendererContext(a_Renderer->context)
    , imageCount(a_Info.imageCount)
    , width(a_Info.width)
    , height(a_Info.height)
    , vSync(a_Info.vSync)
{
    const auto vertCode     = "#version 430                                       \n"
                              "out gl_PerVertex                                   \n"
                              "{                                                  \n"
                              "    vec4 gl_Position;                              \n"
                              "};                                                 \n"
                              "out vec2 UV;                                       \n"
                              "void main() {                                      \n"
                              "   float x = -1.0 + float((gl_VertexID & 1) << 2); \n"
                              "   float y = -1.0 + float((gl_VertexID & 2) << 1); \n"
                              "   UV.x = (x + 1.0) * 0.5;                         \n"
                              "   UV.y = 1 - (y + 1.0) * 0.5;                     \n"
                              "   gl_Position = vec4(x, y, 0, 1);                 \n"
                              "}                                                  \n";
    const auto fragCode     = "#version 430                                           \n"
                              "layout(location = 0) out vec4 out_Color;               \n"
                              "layout(binding = 0) uniform sampler2D in_Color;        \n"
                              "in vec2 UV;                                            \n"
                              "void main() {                                          \n"
                              "   ivec2 coord = ivec2(UV * textureSize(in_Color, 0)); \n"
                              "   out_Color = texelFetch(in_Color, coord, 0);         \n"
                              "}                                                      \n";
    auto& presentVertShader = shaderCompiler.CompileShader(
        GL_VERTEX_SHADER,
        vertCode);
    auto& presentFragShader = shaderCompiler.CompileShader(
        GL_FRAGMENT_SHADER,
        fragCode);
    presentProgram = RAII::MakePtr<RAII::Program>(
        *context, std::vector<RAII::Shader*> { &presentVertShader, &presentFragShader });
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
        while (index < a_OldSwapChain->imageCount && index < imageCount) {
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
        [this,
            currentImage = images.at(imageIndex),
            renderBuffer = a_RenderBuffer]() {
            {
                auto copyWidth  = std::min(width, (*renderBuffer)->width);
                auto copyHeight = std::min(height, (*renderBuffer)->height);
                rendererContext.Wait();
                auto debugGroup = RAII::DebugGroup("Present");
                // wglCopyImageSubDataNV(
                //     HGLRC(rendererContext.hglrc),
                //     **renderBuffer, GL_TEXTURE_2D,
                //     0, 0, 0, 0,
                //     HGLRC(hglrc),
                //     *currentImage, GL_TEXTURE_2D,
                //     0, 0, 0, 0,
                //     copyWidth, copyHeight, 1);
                glBindTexture(GL_TEXTURE_2D, *currentImage);
                glDrawArrays(GL_TRIANGLES, 0, 3);
            }
            glXSwapBuffers((Display*)context->display, context->drawableID);
        });
    context->ExecuteCmds(waitCmds);
    imageIndex = ++imageIndex % imageCount;
}
}
