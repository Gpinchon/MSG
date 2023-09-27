#include <Renderer/OGL/RAII/Buffer.hpp>
#include <Renderer/OGL/RAII/DebugGroup.hpp>
#include <Renderer/OGL/RAII/Program.hpp>
#include <Renderer/OGL/RAII/VertexArray.hpp>
#include <Renderer/OGL/RenderBuffer.hpp>
#include <Renderer/OGL/Win32/Context.hpp>
#include <Renderer/OGL/Win32/Error.hpp>
#include <Renderer/OGL/Win32/SwapChain.hpp>

#include <GL/glew.h>
#include <GL/wglew.h>

namespace TabGraph::Renderer::SwapChain {
Impl::Impl(
    const Renderer::Handle& a_Renderer,
    const CreateSwapChainInfo& a_Info)
    : context(a_Info.hwnd, false)
    , rendererContext(a_Renderer->context)
    , width(a_Info.width)
    , height(a_Info.height)
    , imageCount(a_Info.imageCount)
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
    presentProgram = RAII::MakeWrapper<RAII::Program>(
        context, std::vector<RAII::Shader*> { &presentVertShader, &presentFragShader });
    for (uint8_t index = 0; index < imageCount; ++index)
        images.emplace_back(RAII::MakeWrapper<RAII::Texture2D>(context, width, height, 1, GL_RGB8));
    VertexAttributeDescription attribDesc {};
    attribDesc.binding           = 0;
    attribDesc.format.normalized = false;
    attribDesc.format.size       = 1;
    attribDesc.format.type       = GL_BYTE;
    VertexBindingDescription bindingDesc {};
    bindingDesc.buffer = RAII::MakeWrapper<RAII::Buffer>(context, 3, nullptr, 0);
    bindingDesc.index  = 0;
    bindingDesc.offset = 0;
    bindingDesc.stride = 1;
    std::vector<VertexAttributeDescription> attribs { attribDesc };
    std::vector<VertexBindingDescription> bindings { bindingDesc };
    presentVAO = RAII::MakeWrapper<RAII::VertexArray>(context,
        3, attribs, bindings);
    context.PushRenderCmd([this, vSync = a_Info.vSync] {
        WIN32_CHECK_ERROR(wglSwapIntervalEXT(vSync ? 1 : 0));
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
    , width(a_Info.width)
    , height(a_Info.height)
    , imageCount(a_Info.imageCount)
    , vSync(a_Info.vSync)
    , presentProgram(a_OldSwapChain->presentProgram)
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
            images.emplace_back(RAII::MakeWrapper<RAII::Texture2D>(context, width, height, 1, GL_RGB8));
            ++index;
        }
    }
    context.PushRenderCmd([this, vSync = a_Info.vSync] {
        WIN32_CHECK_ERROR(wglSwapIntervalEXT(vSync ? 1 : 0));
        glViewport(0, 0, width, height);
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
                    *currentImage, GL_TEXTURE_2D,
                    0, 0, 0, 0,
                    copyWidth, copyHeight, 1);
                glBindTexture(GL_TEXTURE_2D, *currentImage);
                glDrawArrays(GL_TRIANGLES, 0, 3);
                glFlush();
            }
            WIN32_CHECK_ERROR(SwapBuffers(HDC(context.hdc)));
        });
    context.ExecuteResourceCreationCmds(true);
    imageIndex = ++imageIndex % imageCount;
}
}
