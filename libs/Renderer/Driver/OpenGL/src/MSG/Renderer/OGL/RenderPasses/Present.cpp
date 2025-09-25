#include <MSG/Renderer/OGL/RenderPasses/Present.hpp>

#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLTexture2D.hpp>
#include <MSG/Renderer/OGL/RenderBuffer.hpp>
#include <MSG/Renderer/OGL/RenderPasses/DfdOpaqueGeometry.hpp>
#include <MSG/Renderer/OGL/RenderPasses/TAA.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Scene.hpp>

#include <Bindings.glsl>

static inline auto CreateFbPresent(
    MSG::OGLContext& a_Context,
    const glm::uvec2& a_Size)
{
    MSG::OGLFrameBufferCreateInfo info;
    info.defaultSize = { a_Size, 1 };
    return std::make_shared<MSG::OGLFrameBuffer>(a_Context, info);
}

MSG::Renderer::Present::Present(Renderer::Impl& a_Renderer)
    : RenderPassInterface({ typeid(TAA) })
    , shader({ .program = a_Renderer.shaderCompiler.CompileProgram("Present") })
{
}

void MSG::Renderer::Present::Update(Renderer::Impl& a_Renderer, const RenderPassesLibrary& a_RenderPasses)
{
    auto& activeScene       = *a_Renderer.activeScene;
    auto& clearColor        = activeScene.GetBackgroundColor();
    auto& renderBuffer      = *a_Renderer.activeRenderBuffer;
    auto renderBufferSize   = glm::uvec3(renderBuffer->width, renderBuffer->height, 1);
    glm::uvec3 internalSize = glm::uvec3(glm::vec2(renderBufferSize) * a_Renderer.internalResolution, 1);
    auto fbPresentSize      = output != nullptr ? output->info.defaultSize : glm::uvec3(0);
    if (fbPresentSize != renderBufferSize) {
        output = CreateFbPresent(a_Renderer.context, renderBufferSize);
        // FILL VIEWPORT STATES
        auto& info                        = renderPassInfo;
        info.name                         = "Present";
        info.viewportState.viewport       = renderBufferSize;
        info.viewportState.scissorExtent  = renderBufferSize;
        info.frameBufferState             = { .framebuffer = output };
        info.frameBufferState.drawBuffers = {};
    }
}

void MSG::Renderer::Present::Render(Impl& a_Renderer)
{
    auto& taaOutput    = a_Renderer.renderPassesLibrary.Get<TAA>().output;
    auto& cmdBuffer    = a_Renderer.renderCmdBuffer;
    auto& renderBuffer = *a_Renderer.activeRenderBuffer;
    // FILL GRAPHICS PIPELINES
    OGLGraphicsPipelineInfo gpInfo;
    gpInfo.depthStencilState  = { .enableDepthTest = false };
    gpInfo.shaderState        = shader;
    gpInfo.inputAssemblyState = { .primitiveTopology = GL_TRIANGLES };
    gpInfo.rasterizationState = { .cullMode = GL_NONE };
    gpInfo.vertexInputState   = { .vertexCount = 3, .vertexArray = a_Renderer.presentVAO };
    gpInfo.bindings.images[0] = { taaOutput->info.colorBuffers[0].texture, GL_READ_ONLY, GL_RGBA16F };
    gpInfo.bindings.images[1] = { renderBuffer, GL_WRITE_ONLY, GL_RGBA8 };
    OGLCmdDrawInfo drawCmd;
    drawCmd.vertexCount = 3;

    cmdBuffer.PushCmd<OGLCmdPushRenderPass>(renderPassInfo);
    cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
    cmdBuffer.PushCmd<OGLCmdDraw>(drawCmd);
    cmdBuffer.PushCmd<OGLCmdEndRenderPass>();
}