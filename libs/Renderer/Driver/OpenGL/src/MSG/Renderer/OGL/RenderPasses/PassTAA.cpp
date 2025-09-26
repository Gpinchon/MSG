#include <MSG/Renderer/OGL/RenderPasses/PassTAA.hpp>

#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLSampler.hpp>
#include <MSG/OGLTexture2D.hpp>
#include <MSG/Renderer/OGL/RenderBuffer.hpp>
#include <MSG/Renderer/OGL/RenderPasses/PassBlendedGeometry.hpp>
#include <MSG/Renderer/OGL/RenderPasses/PassOpaqueGeometry.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Scene.hpp>

#include <Bindings.glsl>

static inline auto CreateFbTemporalAccumulation(
    MSG::OGLContext& a_Context,
    const glm::uvec2& a_Size)
{
    MSG::OGLFrameBufferCreateInfo info;
    info.defaultSize = { a_Size, 1 };
    info.colorBuffers.resize(1);
    info.colorBuffers[0].attachment = GL_COLOR_ATTACHMENT0 + 0;
    info.colorBuffers[0].texture    = std::make_shared<MSG::OGLTexture2D>(a_Context, MSG::OGLTexture2DInfo { .width = a_Size.x, .height = a_Size.y, .levels = 1, .sizedFormat = GL_RGBA16F });
    return std::make_shared<MSG::OGLFrameBuffer>(a_Context, info);
}

MSG::Renderer::PassTAA::PassTAA(Renderer::Impl& a_Renderer)
    : RenderPassInterface({ typeid(PassOpaqueGeometry), typeid(PassBlendedGeometry) })
    , shader(a_Renderer.shaderCompiler.CompileProgram("TemporalAccumulation"))
    , sampler(std::make_shared<OGLSampler>(a_Renderer.context, OGLSamplerParameters { .minFilter = GL_LINEAR, .wrapS = GL_CLAMP_TO_EDGE, .wrapT = GL_CLAMP_TO_EDGE, .wrapR = GL_CLAMP_TO_EDGE }))
{
    frameBuffers[0] = nullptr;
    frameBuffers[1] = nullptr;
}

void MSG::Renderer::PassTAA::Update(Renderer::Impl& a_Renderer, const RenderPassesLibrary& a_RenderPasses)
{
    geometryFB                      = a_RenderPasses.Get<PassOpaqueGeometry>().output;
    auto& activeScene               = *a_Renderer.activeScene;
    auto& clearColor                = activeScene.GetBackgroundColor();
    auto& renderBuffer              = *a_Renderer.activeRenderBuffer;
    auto renderBufferSize           = glm::uvec3(renderBuffer->width, renderBuffer->height, 1);
    glm::uvec3 internalSize         = glm::uvec3(glm::vec2(renderBufferSize) * a_Renderer.settings.internalResolution, 1);
    auto fbTemporalAccumulationSize = frameBuffers[0] != nullptr ? frameBuffers[0]->info.defaultSize : glm::uvec3(0);
    if (fbTemporalAccumulationSize != renderBufferSize) {
        frameBuffers[0]                   = CreateFbTemporalAccumulation(a_Renderer.context, renderBufferSize);
        frameBuffers[1]                   = CreateFbTemporalAccumulation(a_Renderer.context, renderBufferSize);
        auto& info                        = renderPassInfo;
        info.name                         = "TemporalAccumulation";
        info.viewportState.viewport       = renderBufferSize;
        info.viewportState.scissorExtent  = renderBufferSize;
        info.frameBufferState.drawBuffers = { GL_COLOR_ATTACHMENT0 };
        firstFrame                        = true;
    }
}

void MSG::Renderer::PassTAA::Render(Impl& a_Renderer)
{
    auto& cmdBuffer = a_Renderer.renderCmdBuffer;
    output          = frameBuffers[(a_Renderer.frameIndex + 0) % 2];
    output_Previous = frameBuffers[(a_Renderer.frameIndex + 1) % 2];
    // FILL VIEWPORT STATES
    renderPassInfo.frameBufferState.framebuffer = output;
    // FILL GRAPHICS PIPELINES
    auto color_Previous = firstFrame ? geometryFB->info.colorBuffers[OUTPUT_FRAG_DFD_FINAL].texture : output_Previous->info.colorBuffers[0].texture;
    firstFrame          = false;

    OGLGraphicsPipelineInfo gpInfo;
    gpInfo.depthStencilState    = { .enableDepthTest = false };
    gpInfo.shaderState.program  = shader;
    gpInfo.inputAssemblyState   = { .primitiveTopology = GL_TRIANGLES };
    gpInfo.rasterizationState   = { .cullMode = GL_NONE };
    gpInfo.vertexInputState     = { .vertexCount = 3, .vertexArray = a_Renderer.presentVAO };
    gpInfo.bindings.textures[0] = { .texture = color_Previous, .sampler = sampler };
    gpInfo.bindings.textures[1] = { .texture = geometryFB->info.colorBuffers[OUTPUT_FRAG_DFD_FINAL].texture, .sampler = sampler };
    gpInfo.bindings.textures[2] = { .texture = geometryFB->info.colorBuffers[OUTPUT_FRAG_DFD_VELOCITY].texture, .sampler = sampler };
    OGLCmdDrawInfo drawCmd;
    drawCmd.vertexCount = 3;

    cmdBuffer.PushCmd<OGLCmdPushRenderPass>(renderPassInfo);
    cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
    cmdBuffer.PushCmd<OGLCmdDraw>(drawCmd);
    cmdBuffer.PushCmd<OGLCmdEndRenderPass>();
}