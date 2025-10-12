#include <MSG/Renderer/OGL/TextureBlurHelper.hpp>

#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Tools/Pi.hpp>

#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLSampler.hpp>
#include <MSG/OGLTexture2D.hpp>
#include <MSG/OGLTextureView.hpp>
#include <MSG/OGLTypedBuffer.hpp>

#include <GaussianBlur.glsl>

#include <glm/gtx/matrix_transform_2d.hpp>

Msg::Renderer::TextureBlurHelper::TextureBlurHelper(Msg::Renderer::Impl& a_Renderer, const std::shared_ptr<Msg::OGLTexture>& a_Target)
    : sampler(std::make_shared<Msg::OGLSampler>(a_Renderer.context, Msg::OGLSamplerParameters { .wrapS = GL_CLAMP_TO_EDGE, .wrapT = GL_CLAMP_TO_EDGE, .wrapR = GL_CLAMP_TO_EDGE }))
    , settingsBuffer(std::make_shared<Msg::OGLTypedBufferArray<Msg::Renderer::GLSL::GaussianBlurSettingsUBO>>(a_Renderer.context, GAUSSIAN_BLUR_PASS_COUNT))
{
    shader = *a_Renderer.shaderCache["GaussianBlur"];
    if (!shader)
        shader = a_Renderer.shaderCompiler.CompileProgram("GaussianBlur");
    tempTexture = std::make_shared<Msg::OGLTexture2D>(a_Renderer.context,
        Msg::OGLTexture2DInfo {
            .width       = a_Target->width,
            .height      = a_Target->height,
            .sizedFormat = a_Target->sizedFormat,
        });
    for (uint8_t pass = 0; pass < GAUSSIAN_BLUR_PASS_COUNT; pass++) {
        constexpr float radSplits = M_PI / float(GAUSSIAN_BLUR_PASS_COUNT);
        float angle               = radSplits * pass;
        Msg::Renderer::GLSL::GaussianBlurSettings settings {
            .direction = glm::vec3(glm::vec2(1, 0), 0.f) * glm::rotate(glm::mat3(1), angle),
            .scale     = glm::vec2(1)
        };
        settingsBuffer->Set(pass, Msg::Renderer::GLSL::GaussianBlurSettingsUBO(settings));
    }
    settingsBuffer->Update();
    for (uint32_t layer = 0; layer < a_Target->depth; layer++) {
        Msg::OGLTextureViewInfo viewInfo {
            .target      = GL_TEXTURE_2D,
            .minLayer    = layer,
            .sizedFormat = a_Target->sizedFormat
        };
        Msg::OGLFrameBufferCreateInfo fbInfo;
        fbInfo.colorBuffers.resize(2);
        fbInfo.defaultSize                = { a_Target->width, a_Target->height, 1 };
        fbInfo.colorBuffers[0].texture    = tempTexture;
        fbInfo.colorBuffers[0].attachment = GL_COLOR_ATTACHMENT0;
        fbInfo.colorBuffers[1].texture    = std::make_shared<Msg::OGLTextureView>(a_Renderer.context, a_Target, viewInfo);
        fbInfo.colorBuffers[1].attachment = GL_COLOR_ATTACHMENT1;
        fbs.emplace_back(std::make_shared<Msg::OGLFrameBuffer>(a_Renderer.context, fbInfo));
    }
}

void Msg::Renderer::TextureBlurHelper::operator()(Msg::Renderer::Impl& a_Renderer, Msg::OGLCmdBuffer& a_CmdBuffer, const float& a_Radius)
{
    for (uint8_t pass = 0; pass < GAUSSIAN_BLUR_PASS_COUNT; pass++) {
        Msg::Renderer::GLSL::GaussianBlurSettingsUBO passSettings = settingsBuffer->Get(pass);
        passSettings.scale                                        = glm::vec2(a_Radius);
        settingsBuffer->Set(pass, passSettings);
    }
    settingsBuffer->Update();
    for (auto& fb : fbs) { // blur each layer
        for (uint8_t pass = 0; pass < GAUSSIAN_BLUR_PASS_COUNT; pass++) {
            Msg::OGLRenderPassInfo passInfo;
            passInfo.frameBufferState.framebuffer = fb;
            passInfo.frameBufferState.drawBuffers = { GLenum((pass % 2) == 0 ? GL_COLOR_ATTACHMENT0 : GL_COLOR_ATTACHMENT1) };
            passInfo.viewportState.viewport       = { tempTexture->width, tempTexture->height };
            passInfo.viewportState.scissorExtent  = passInfo.viewportState.viewport;
            Msg::OGLGraphicsPipelineInfo gpInfo;
            gpInfo.depthStencilState.enableDepthTest = false;
            gpInfo.shaderState.program               = shader;
            gpInfo.inputAssemblyState                = { .primitiveTopology = GL_TRIANGLES };
            gpInfo.rasterizationState                = { .cullMode = GL_NONE };
            gpInfo.vertexInputState                  = { .vertexCount = 3, .vertexArray = a_Renderer.presentVAO };
            gpInfo.bindings.textures[0].texture      = fb->info.colorBuffers[(pass + 1) % 2].texture;
            gpInfo.bindings.textures[0].sampler      = sampler;
            gpInfo.bindings.uniformBuffers[0].buffer = settingsBuffer;
            gpInfo.bindings.uniformBuffers[0].size   = settingsBuffer->value_size;
            gpInfo.bindings.uniformBuffers[0].offset = settingsBuffer->value_size * pass;
            Msg::OGLCmdDrawInfo drawCmd;
            drawCmd.vertexCount = 3;
            a_CmdBuffer.PushCmd<Msg::OGLCmdPushRenderPass>(passInfo);
            a_CmdBuffer.PushCmd<Msg::OGLCmdPushPipeline>(gpInfo);
            a_CmdBuffer.PushCmd<Msg::OGLCmdDraw>(drawCmd);
            a_CmdBuffer.PushCmd<Msg::OGLCmdEndRenderPass>();
        }
    }
}