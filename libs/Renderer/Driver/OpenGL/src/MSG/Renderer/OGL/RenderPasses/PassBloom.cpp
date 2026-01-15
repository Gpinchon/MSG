#include <MSG/Renderer/OGL/RenderPasses/PassBloom.hpp>

#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLPipelineInfo.hpp>
#include <MSG/OGLTexture2D.hpp>
#include <MSG/OGLTypedBuffer.hpp>
#include <MSG/Renderer/OGL/RenderBuffer.hpp>
#include <MSG/Renderer/OGL/RenderPasses/PassOpaqueGeometry.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/Subsystems/MeshSubsystem.hpp>

#include <MSG/Camera.hpp>
#include <MSG/Scene.hpp>

#include <Bindings.glsl>
#include <Bloom.glsl>

static inline Msg::Renderer::GLSL::BloomSettings GetBloomSettings(const Msg::CameraBloomSettings& a_Settings)
{
    return Msg::Renderer::GLSL::BloomSettings {
        .intensity = a_Settings.intensity,
        .threshold = a_Settings.threshold,
        .smoothing = a_Settings.smoothing,
        .size      = a_Settings.size,
        .tint      = a_Settings.tint,
    };
}

Msg::Renderer::PassBloom::PassBloom(Renderer::Impl& a_Renderer)
    : settingsBuffer(std::make_shared<OGLTypedBuffer<GLSL::BloomSettings>>(a_Renderer.context, GetBloomSettings({})))
    , compositingShader(a_Renderer.shaderCompiler.CompileProgram("BloomCompositing"))
    , lightExtractionShader(a_Renderer.shaderCompiler.CompileProgram("BloomLightExtraction"))
{
}

void Msg::Renderer::PassBloom::UpdateSettings(Renderer::Impl& a_Renderer, const Renderer::RendererSettings& a_Settings)
{
}

void Msg::Renderer::PassBloom::Update(Renderer::Impl& a_Renderer, const RenderPassesLibrary& a_RenderPasses)
{
    geometryFB              = a_RenderPasses.Get<PassOpaqueGeometry>().output;
    auto& scene             = *a_Renderer.activeScene;
    auto& camera            = scene.GetCamera().GetComponent<Msg::Camera>();
    auto& renderBuffer      = *a_Renderer.activeRenderBuffer;
    auto renderBufferSize   = glm::uvec3(renderBuffer->width, renderBuffer->height, 1);
    glm::uvec3 internalSize = glm::uvec3(glm::vec2(renderBufferSize) * a_Renderer.settings.internalResolution, 1);
    auto fbSize             = lightTexture != nullptr ? glm::uvec3(lightTexture->width, lightTexture->height, lightTexture->depth) : glm::uvec3(0);
    settingsBuffer->Set(GetBloomSettings(camera.settings.bloom));
    settingsBuffer->Update();
    if (fbSize != internalSize) {
        lightTexture = std::make_shared<OGLTexture2D>(
            a_Renderer.context,
            OGLTexture2DInfo {
                .width       = internalSize.x,
                .height      = internalSize.y,
                .sizedFormat = GL_RGB16F,
            });
        OGLFrameBufferCreateInfo fbInfo;
        fbInfo.defaultSize = internalSize;
        fbInfo.colorBuffers.resize(1);
        fbInfo.colorBuffers[0] = { .attachment = GL_COLOR_ATTACHMENT0, .layer = 0, .texture = lightTexture };
        lightExtractionFB      = std::make_shared<OGLFrameBuffer>(a_Renderer.context, fbInfo);
    }
}

void Msg::Renderer::PassBloom::Render(Impl& a_Renderer)
{
    auto& activeScene = *a_Renderer.activeScene;
    auto& cmdBuffer   = a_Renderer.renderCmdBuffer;
    auto& blurHelper  = a_Renderer.blurHelpers.Get(a_Renderer, lightTexture);
    {
        OGLRenderPassInfo passInfo;
        OGLGraphicsPipelineInfo gpInfo;
        OGLCmdDrawInfo drawCmd;
        passInfo.name                         = "Bloom::LightExtraction";
        passInfo.viewportState.viewportExtent = lightExtractionFB->info.defaultSize;
        passInfo.viewportState.scissorExtent  = lightExtractionFB->info.defaultSize;
        passInfo.frameBufferState.framebuffer = lightExtractionFB;
        passInfo.frameBufferState.drawBuffers = { GL_COLOR_ATTACHMENT0 };
        gpInfo.inputAssemblyState             = { .primitiveTopology = GL_TRIANGLES };
        gpInfo.rasterizationState             = { .cullMode = GL_NONE };
        gpInfo.vertexInputState               = { .vertexCount = 3, .vertexArray = a_Renderer.presentVAO };
        gpInfo.bindings.textures[0]           = { .texture = geometryFB->info.colorBuffers[OUTPUT_FRAG_DFD_FINAL].texture };
        gpInfo.bindings.uniformBuffers[0]     = { .buffer = settingsBuffer, .offset = 0, .size = settingsBuffer->size };
        gpInfo.shaderState.program            = lightExtractionShader;
        drawCmd.vertexCount                   = 3;
        cmdBuffer.PushCmd<OGLCmdPushRenderPass>(passInfo);
        cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
        cmdBuffer.PushCmd<OGLCmdDraw>(drawCmd);
        cmdBuffer.PushCmd<OGLCmdEndRenderPass>();
    }
    blurHelper(a_Renderer, cmdBuffer, settingsBuffer->Get().size);
    {
        OGLRenderPassInfo passInfo;
        OGLGraphicsPipelineInfo gpInfo;
        OGLCmdDrawInfo drawCmd;
        passInfo.name                         = "Bloom::Compositing";
        passInfo.viewportState.viewportExtent = geometryFB->info.defaultSize;
        passInfo.viewportState.scissorExtent  = geometryFB->info.defaultSize;
        passInfo.frameBufferState.framebuffer = geometryFB;
        passInfo.frameBufferState.drawBuffers = { GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_DFD_FINAL };
        gpInfo.shaderState.program            = compositingShader;
        gpInfo.inputAssemblyState             = { .primitiveTopology = GL_TRIANGLES };
        gpInfo.rasterizationState             = { .cullMode = GL_NONE };
        gpInfo.vertexInputState               = { .vertexCount = 3, .vertexArray = a_Renderer.presentVAO };
        gpInfo.bindings.textures[0]           = { .texture = lightTexture };
        gpInfo.bindings.uniformBuffers[0]     = { .buffer = settingsBuffer, .offset = 0, .size = settingsBuffer->size };
        gpInfo.colorBlend.attachmentStates.resize(1);
        gpInfo.colorBlend.attachmentStates[0].index               = 0;
        gpInfo.colorBlend.attachmentStates[0].enableBlend         = true;
        gpInfo.colorBlend.attachmentStates[0].srcColorBlendFactor = GL_ONE;
        gpInfo.colorBlend.attachmentStates[0].dstColorBlendFactor = GL_ONE;
        gpInfo.colorBlend.attachmentStates[0].colorBlendOp        = GL_FUNC_ADD;
        gpInfo.colorBlend.attachmentStates[0].srcAlphaBlendFactor = GL_ZERO;
        gpInfo.colorBlend.attachmentStates[0].dstAlphaBlendFactor = GL_ONE;
        gpInfo.colorBlend.attachmentStates[0].alphaBlendOp        = GL_FUNC_ADD;
        drawCmd.vertexCount                                       = 3;
        cmdBuffer.PushCmd<OGLCmdPushRenderPass>(passInfo);
        cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
        cmdBuffer.PushCmd<OGLCmdDraw>(drawCmd);
        cmdBuffer.PushCmd<OGLCmdEndRenderPass>();
    }
}