#include <MSG/Camera/Projection.hpp>
#include <MSG/Entity/Node.hpp>
#include <MSG/Light/PunctualLight.hpp>
#include <MSG/OGLBindlessTextureSampler.hpp>
#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLPipeline.hpp>
#include <MSG/OGLRenderPass.hpp>
#include <MSG/OGLSampler.hpp>
#include <MSG/OGLTexture2DArray.hpp>
#include <MSG/OGLTextureCube.hpp>
#include <MSG/Renderer/OGL/Components/LightShadowData.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Scene.hpp>
#include <MSG/SphericalHarmonics.hpp>
#include <MSG/Texture.hpp>

#include <GL/glew.h>

#include <Functions.glsl>

namespace Msg::Renderer {
GLenum GetShadowDepthPixelFormat(const LightShadowPrecision& a_Precision)
{
    switch (a_Precision) {
    case LightShadowPrecision::High:
        return GL_DEPTH_COMPONENT32;
    case LightShadowPrecision::Medium:
        return GL_DEPTH_COMPONENT24;
    case LightShadowPrecision::Low:
        return GL_DEPTH_COMPONENT16;
    }
    return GL_NONE;
}

std::shared_ptr<OGLTexture> CreateTextureMinMax(OGLContext& a_Ctx, const LightShadowSettings& a_ShadowSettings, const size_t a_ViewportsCount)
{
    OGLTexture2DArrayInfo info {
        .width       = a_ShadowSettings.resolution,
        .height      = a_ShadowSettings.resolution,
        .layers      = uint32_t(a_ViewportsCount),
        .levels      = uint32_t(MIPMAPNBR(glm::ivec2(a_ShadowSettings.resolution))),
        .sizedFormat = GL_RG32F
    };
    return std::make_shared<OGLTexture2DArray>(a_Ctx, info);
}

std::shared_ptr<OGLTexture> CreateTextureMinMaxPoint(OGLContext& a_Ctx, const LightShadowSettings& a_ShadowSettings)
{
    OGLTextureCubeInfo info {
        .width       = a_ShadowSettings.resolution,
        .height      = a_ShadowSettings.resolution,
        .levels      = uint32_t(MIPMAPNBR(glm::ivec2(a_ShadowSettings.resolution))),
        .sizedFormat = GL_RG32F
    };
    return std::make_shared<OGLTextureCube>(a_Ctx, info);
}

std::shared_ptr<OGLTexture> CreateTextureDepth(OGLContext& a_Ctx, const LightShadowSettings& a_ShadowSettings, const size_t a_ViewportsCount)
{
    OGLTexture2DArrayInfo info {
        .width       = a_ShadowSettings.resolution,
        .height      = a_ShadowSettings.resolution,
        .layers      = uint32_t(a_ViewportsCount),
        .sizedFormat = GetShadowDepthPixelFormat(a_ShadowSettings.precision)
    };
    return std::make_shared<OGLTexture2DArray>(a_Ctx, info);
}

std::shared_ptr<OGLTexture> CreateTextureDepthPoint(OGLContext& a_Ctx, const LightShadowSettings& a_ShadowSettings)
{
    OGLTextureCubeInfo info {
        .width       = a_ShadowSettings.resolution,
        .height      = a_ShadowSettings.resolution,
        .sizedFormat = GetShadowDepthPixelFormat(a_ShadowSettings.precision)
    };
    return std::make_shared<OGLTextureCube>(a_Ctx, info);
}

auto CreateShadowSampler(Msg::OGLContext& a_Ctx)
{
    Msg::OGLSamplerParameters parameters;
    parameters.minFilter   = GL_LINEAR;
    parameters.wrapS       = GL_CLAMP_TO_BORDER;
    parameters.wrapT       = GL_CLAMP_TO_BORDER;
    parameters.wrapR       = GL_CLAMP_TO_BORDER;
    parameters.compareMode = GL_COMPARE_REF_TO_TEXTURE;
    parameters.compareFunc = GL_LEQUAL;
    parameters.borderColor = glm::vec4(1);
    return std::make_shared<Msg::OGLSampler>(a_Ctx, parameters);
}

auto CreateShadowSamplerPoint(Msg::OGLContext& a_Ctx)
{
    Msg::OGLSamplerParameters parameters;
    parameters.seamlessCubemap = true;
    parameters.minFilter       = GL_LINEAR;
    parameters.wrapS           = GL_CLAMP_TO_EDGE;
    parameters.wrapT           = GL_CLAMP_TO_EDGE;
    parameters.wrapR           = GL_CLAMP_TO_EDGE;
    parameters.compareMode     = GL_COMPARE_REF_TO_TEXTURE;
    parameters.compareFunc     = GL_LEQUAL;
    return std::make_shared<Msg::OGLSampler>(a_Ctx, parameters);
}

Msg::Renderer::LightShadowData::LightShadowData(Renderer::Impl& a_Rdr)
    : bufferDepthRange(std::make_shared<OGLTypedBufferArray<float>>(a_Rdr.context, 4))
{
    bufferDepthRange->Set(0, 0);
    bufferDepthRange->Set(1, 1);
    bufferDepthRange->Update();
}

void LightShadowData::Update(Renderer::Impl& a_Rdr,
    const LightType& a_LightType,
    const LightShadowSettings& a_ShadowSettings,
    const size_t& a_ViewportCount)
{
    if (textureSampler == nullptr || textureSampler->texture->height != a_ShadowSettings.resolution)
        _UpdateTextureSampler(a_Rdr, a_LightType, a_ShadowSettings, a_ViewportCount);
    UpdateDepthRange(a_Rdr, a_LightType);
}
}

void Msg::Renderer::LightShadowData::UpdateDepthRange(Renderer::Impl& a_Rdr,
    const LightType& a_LightType)
{
    if (!needsUpdate)
        return;
    needsUpdate = false;
    OGLCmdBuffer cmdBuffer(a_Rdr.context);
    OGLRenderPassInfo renderPass;
    renderPass.name                         = "ShadowHZB depth copy";
    renderPass.frameBufferState.framebuffer = frameBufferHZB;
    renderPass.viewportState.viewportExtent = { textureHZB->width, textureHZB->height };
    renderPass.viewportState.scissorExtent  = { textureHZB->width, textureHZB->height };
    bool isCube                             = a_LightType == LightType::Point;
    OGLGraphicsPipelineInfo pipeline;
    pipeline.shaderState.program = a_Rdr.shaderCompiler.CompileProgram("ShadowHZB", ShaderLibrary::ProgramKeywords { { "CUBE", isCube ? "1" : "0" } });
    pipeline.depthStencilState   = { .enableDepthTest = false, .enableDepthWrite = false };
    pipeline.inputAssemblyState  = { .primitiveTopology = GL_TRIANGLES };
    pipeline.rasterizationState  = { .cullMode = GL_NONE };
    pipeline.vertexInputState    = { .vertexCount = 3, .vertexArray = a_Rdr.presentVAO };
    OGLCmdDrawInfo drawCmd;
    drawCmd.vertexCount = 3;
    cmdBuffer.Begin();
    for (int level = 0; level < textureHZB->levels - 1; level++) {
        renderPass.name = "ShadowHZB_" + std::to_string(level);
        renderPass.viewportState.viewportExtent /= 2;
        renderPass.viewportState.scissorExtent /= 2;
        pipeline.bindings.images[0] = OGLImageBindingInfo { .texture = textureHZB, .access = GL_READ_ONLY, .format = GL_RG32F, .level = level, .layered = true };
        pipeline.bindings.images[1] = OGLImageBindingInfo { .texture = textureHZB, .access = GL_WRITE_ONLY, .format = GL_RG32F, .level = level + 1, .layered = true };
        cmdBuffer.PushCmd<OGLCmdPushRenderPass>(renderPass);
        cmdBuffer.PushCmd<OGLCmdPushPipeline>(pipeline);
        cmdBuffer.PushCmd<OGLCmdDraw>(drawCmd);
        cmdBuffer.PushCmd<OGLCmdEndRenderPass>();
    }
    // cmdBuffer.PushCmd<OGLCmdMemoryBarrier>(GL_TEXTURE_UPDATE_BARRIER_BIT);
    cmdBuffer.End();
    cmdBuffer.Execute();
    std::vector<glm::vec2> txtData(textureHZB->depth);
    textureHZB->DownloadLevel(
        textureHZB->levels - 1,
        GL_RG, GL_FLOAT,
        sizeof(glm::vec2) * txtData.size(), txtData.data());
    float minVal = txtData[0][0];
    float maxVal = txtData[0][1];
    for (uint32_t i = 1; i < textureHZB->depth; i++) {
        minVal = glm::min(txtData[i][0], minVal);
        maxVal = glm::max(txtData[i][1], maxVal);
    }
    assert(minVal != std::numeric_limits<float>::quiet_NaN());
    assert(maxVal != std::numeric_limits<float>::quiet_NaN());
    if (minVal == 0 && maxVal == 0)
        return;
    // slightly increase min/max depth to avoid z fighting with near and far planes
    minVal = glm::max(0.f, minVal - 0.00001f);
    maxVal = maxVal + 0.00001f;
    // use rolling average to avoid sudden jumps
    minDepth = glm::mix(minVal, minDepth, 0.8);
    maxDepth = glm::mix(maxVal, maxDepth, 0.8);
    bufferDepthRange->Set(0, minDepth);
    bufferDepthRange->Set(1, maxDepth);
    bufferDepthRange->Update();
}

void Msg::Renderer::LightShadowData::_UpdateTextureSampler(Renderer::Impl& a_Rdr,
    const LightType& a_LightType,
    const LightShadowSettings& a_ShadowSettings,
    const size_t& a_ViewportCount)
{
    auto textureDepth = a_LightType == LightType::Point ? CreateTextureDepthPoint(a_Rdr.context, a_ShadowSettings) : CreateTextureDepth(a_Rdr.context, a_ShadowSettings, a_ViewportCount);
    auto samplerDepth = a_LightType == LightType::Point ? CreateShadowSamplerPoint(a_Rdr.context) : CreateShadowSampler(a_Rdr.context);
    textureHZB        = a_LightType == LightType::Point ? CreateTextureMinMaxPoint(a_Rdr.context, a_ShadowSettings) : CreateTextureMinMax(a_Rdr.context, a_ShadowSettings, a_ViewportCount);
    textureSampler    = std::make_shared<OGLBindlessTextureSampler>(a_Rdr.context, textureDepth, samplerDepth);
    frameBuffer       = std::make_shared<OGLFrameBuffer>(a_Rdr.context,
              OGLFrameBufferCreateInfo {
                  .layered      = false,
                  .defaultSize  = { textureDepth->width, textureDepth->height, textureDepth->depth },
                  .colorBuffers = { { .attachment = GL_COLOR_ATTACHMENT0, .texture = textureHZB } },
                  .depthBuffer  = { .texture = textureDepth },
        });
    frameBufferHZB    = std::make_shared<OGLFrameBuffer>(a_Rdr.context,
           OGLFrameBufferCreateInfo {
               .layered     = false,
               .defaultSize = { textureHZB->width, textureHZB->height, 1 } });
}
