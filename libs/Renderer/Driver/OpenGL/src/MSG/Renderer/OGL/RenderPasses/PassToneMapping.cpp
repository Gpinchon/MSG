#include <MSG/Renderer/OGL/RenderPasses/PassOpaqueGeometry.hpp>
#include <MSG/Renderer/OGL/RenderPasses/PassToneMapping.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>

#include <MSG/OGLBuffer.hpp>
#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLPipelineInfo.hpp>
#include <MSG/OGLRenderPassInfo.hpp>
#include <MSG/OGLTexture.hpp>
#include <MSG/OGLTypedBuffer.hpp>

#include <Bindings.glsl>
#include <Exposure.glsl>
#include <Functions.glsl>
#include <Tonemapping.glsl>

auto GetAutoExposureSettings(const Msg::Renderer::AutoExposureSettings& a_Settings, const float& a_DeltaTime)
{
    float minLogLum = log(a_Settings.minLuminance);
    float maxLogLum = log(a_Settings.maxLuminance);
    return Msg::Renderer::GLSL::AutoExposureSettings {
        .minLogLum = minLogLum,
        .maxLogLum = maxLogLum,
        .key       = a_Settings.key,
        .deltaTime = a_DeltaTime * a_Settings.adaptationSpeed,
    };
}

auto GetTonemapSettings(const Msg::Renderer::ToneMappingSettings& a_Settings)
{
    return Msg::Renderer::GLSL::ToneMappingSettings {
        .exposure   = a_Settings.exposure,
        .saturation = a_Settings.saturation,
        .contrast   = a_Settings.contrast,
        .gamma      = a_Settings.gamma,
    };
}

auto CreateAutoExposureSettingsBuffer(Msg::Renderer::Impl& a_Renderer)
{
    return std::make_shared<Msg::OGLTypedBuffer<Msg::Renderer::GLSL::AutoExposureSettings>>(
        a_Renderer.context);
}

auto CreateToneMappingSettingsBuffer(Msg::Renderer::Impl& a_Renderer)
{
    return std::make_shared<Msg::OGLTypedBuffer<Msg::Renderer::GLSL::ToneMappingSettings>>(
        a_Renderer.context);
}

auto CreateLuminanceBuffer(Msg::Renderer::Impl& a_Renderer)
{
    float defaultVal[4];
    defaultVal[0] = 0.5;
    return std::make_shared<Msg::OGLBuffer>(
        a_Renderer.context,
        sizeof(float) * 4,
        defaultVal,
        0);
}

auto CreateLuminanceTexture(Msg::Renderer::Impl& a_Renderer)
{
    glm::ivec2 texSize = { 64, 64 };
    return std::make_shared<Msg::OGLTexture>(
        a_Renderer.context,
        Msg::OGLTextureInfo {
            .target      = GL_TEXTURE_2D,
            .width       = 64,
            .height      = 64,
            .depth       = 1,
            .levels      = uint32_t(MIPMAPNBR(texSize)),
            .sizedFormat = GL_R16F });
}

void Msg::Renderer::PassToneMapping::Update(Renderer::Impl& a_Renderer, const RenderPassesLibrary& a_Subsystems)
{
    bool autoExposure = a_Renderer.settings.toneMapping.autoExposure.enabled;
    auto& pass        = a_Subsystems.Get<PassOpaqueGeometry>();
    auto& tgt         = pass.output->info.colorBuffers[OUTPUT_FRAG_DFD_FINAL].texture;
    if (toneMappingFB == nullptr || toneMappingFB->info.defaultSize.x != tgt->width || toneMappingFB->info.defaultSize.y != tgt->height) {
        OGLFrameBufferCreateInfo fbInfo;
        fbInfo.defaultSize = { tgt->width, tgt->height, tgt->depth };
        toneMappingFB      = std::make_shared<OGLFrameBuffer>(a_Renderer.context, fbInfo);
    }
    toneMappingSettings->Set(GetTonemapSettings(a_Renderer.settings.toneMapping));
    toneMappingSettings->Update();
    cmdBuffer.Reset();
    cmdBuffer.Begin();
    if (autoExposure) {
        const auto now           = std::chrono::steady_clock::now();
        const auto histDeltaTime = std::chrono::duration<float>(now - lastHistUpdate).count();
        const auto deltaTime     = std::chrono::duration<float, std::milli>(now - lastUpdate).count();
        lastUpdate               = now;
        autoExposureSettings->Set(GetAutoExposureSettings(a_Renderer.settings.toneMapping.autoExposure, deltaTime / 1000.f));
        autoExposureSettings->Update();
        if (histDeltaTime >= 0.25) {
            lastHistUpdate = now;
            // Luminance Extraction
            {
                OGLRenderPassInfo renderPass;
                renderPass.frameBufferState.framebuffer = luminanceExtractionFB;
                renderPass.frameBufferState.drawBuffers = { GL_COLOR_ATTACHMENT0 };
                renderPass.viewportState.viewport       = { luminanceTex->width, luminanceTex->height };
                renderPass.viewportState.scissorExtent  = { luminanceTex->width, luminanceTex->height };
                OGLGraphicsPipelineInfo pipeline;
                pipeline.shaderState.program        = a_Renderer.shaderCompiler.CompileProgram("LumExtraction");
                pipeline.bindings.textures[0]       = { .texture = tgt };
                pipeline.bindings.uniformBuffers[0] = { .buffer = autoExposureSettings, .offset = 0, .size = autoExposureSettings->size };
                pipeline.inputAssemblyState         = { .primitiveTopology = GL_TRIANGLES };
                pipeline.rasterizationState         = { .cullMode = GL_NONE };
                pipeline.vertexInputState           = { .vertexCount = 3, .vertexArray = a_Renderer.presentVAO };
                OGLCmdDrawInfo drawCmd;
                drawCmd.vertexCount = 3;
                cmdBuffer.PushCmd<OGLCmdPushRenderPass>(renderPass);
                cmdBuffer.PushCmd<OGLCmdPushPipeline>(pipeline);
                cmdBuffer.PushCmd<OGLCmdDraw>(drawCmd);
                cmdBuffer.PushCmd<OGLCmdEndRenderPass>();
                cmdBuffer.PushCmd<OGLCmdGenerateMipmap>(luminanceTex);
            }
        }
        // Luminance Average
        {
            OGLComputePipelineInfo pipeline;
            pipeline.bindings.textures[0]       = { .texture = luminanceTex };
            pipeline.bindings.uniformBuffers[0] = { .buffer = autoExposureSettings, .offset = 0, .size = autoExposureSettings->size };
            pipeline.bindings.storageBuffers[0] = { .buffer = luminance, .offset = 0, .size = luminance->size };
            pipeline.shaderState.program        = a_Renderer.shaderCompiler.CompileProgram("LumAverage");
            cmdBuffer.PushCmd<OGLCmdPushPipeline>(pipeline);
            cmdBuffer.PushCmd<OGLCmdDispatchCompute>(OGLCmdDispatchComputeInfo { 1, 1, 1 });
            cmdBuffer.PushCmd<OGLCmdMemoryBarrier>(GL_SHADER_STORAGE_BARRIER_BIT);
        }
    }
    // ToneMapping
    {
        auto program = a_Renderer.shaderCompiler.CompileProgram("ToneMapping", ShaderLibrary::ProgramKeywords { { "AUTO_EXPOSURE", autoExposure ? "1" : "0" } });
        OGLRenderPassInfo renderPass;
        renderPass.frameBufferState.framebuffer = toneMappingFB;
        renderPass.viewportState.viewport       = { tgt->width, tgt->height };
        renderPass.viewportState.scissorExtent  = { tgt->width, tgt->height };
        OGLGraphicsPipelineInfo pipeline;
        pipeline.shaderState.program        = program;
        pipeline.bindings.images[0]         = { .texture = tgt, .access = GL_READ_WRITE, .format = GL_RGBA16F };
        pipeline.bindings.uniformBuffers[0] = { .buffer = autoExposureSettings, .offset = 0, .size = autoExposureSettings->size };
        pipeline.bindings.uniformBuffers[1] = { .buffer = toneMappingSettings, .offset = 0, .size = toneMappingSettings->size };
        pipeline.bindings.uniformBuffers[2] = { .buffer = luminance, .offset = 0, .size = luminance->size };
        pipeline.inputAssemblyState         = { .primitiveTopology = GL_TRIANGLES };
        pipeline.rasterizationState         = { .cullMode = GL_NONE };
        pipeline.vertexInputState           = { .vertexCount = 3, .vertexArray = a_Renderer.presentVAO };
        OGLCmdDrawInfo drawCmd;
        drawCmd.vertexCount = 3;
        cmdBuffer.PushCmd<OGLCmdPushRenderPass>(renderPass);
        cmdBuffer.PushCmd<OGLCmdPushPipeline>(pipeline);
        cmdBuffer.PushCmd<OGLCmdDraw>(drawCmd);
        cmdBuffer.PushCmd<OGLCmdEndRenderPass>();
        cmdBuffer.PushCmd<OGLCmdMemoryBarrier>(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }
    cmdBuffer.End();
}

void Msg::Renderer::PassToneMapping::Render(Impl& a_Renderer)
{
    a_Renderer.renderCmdBuffer.PushCmd<OGLCmdPushCmdBuffer>(cmdBuffer);
}

Msg::Renderer::PassToneMapping::PassToneMapping(Renderer::Impl& a_Renderer)
    : RenderPassInterface({ typeid(PassOpaqueGeometry) })
    , cmdBuffer(a_Renderer.context)
    , luminanceTex(CreateLuminanceTexture(a_Renderer))
    , luminance(CreateLuminanceBuffer(a_Renderer))
    , autoExposureSettings(CreateAutoExposureSettingsBuffer(a_Renderer))
    , toneMappingSettings(CreateToneMappingSettingsBuffer(a_Renderer))
{
    OGLFrameBufferCreateInfo fbInfo;
    fbInfo.defaultSize = { luminanceTex->width, luminanceTex->height, luminanceTex->depth };
    fbInfo.colorBuffers.emplace_back(OGLFrameBufferAttachment {
        .attachment = GL_COLOR_ATTACHMENT0,
        .layer      = 0,
        .texture    = luminanceTex,
    });
    luminanceExtractionFB = std::make_shared<OGLFrameBuffer>(a_Renderer.context, fbInfo);
}
