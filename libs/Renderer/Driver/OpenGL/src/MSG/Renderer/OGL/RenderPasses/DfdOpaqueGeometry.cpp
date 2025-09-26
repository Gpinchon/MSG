#include <MSG/Renderer/OGL/RenderPasses/DfdOpaqueGeometry.hpp>

#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLTexture2D.hpp>
#include <MSG/Renderer/OGL/RenderBuffer.hpp>
#include <MSG/Renderer/OGL/RenderPasses/DfdSubPassOpaqueGeometry.hpp>
#include <MSG/Renderer/OGL/RenderPasses/DfdSubPassSkybox.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/Subsystems/MeshSubsystem.hpp>
#include <MSG/Scene.hpp>

#include <Bindings.glsl>

static inline auto CreateFbGeometry(
    MSG::OGLContext& a_Context,
    const glm::uvec2& a_Size)
{
    auto depthStencilTexture = std::make_shared<MSG::OGLTexture2D>(a_Context, MSG::OGLTexture2DInfo { .width = a_Size.x, .height = a_Size.y, .levels = 1, .sizedFormat = GL_DEPTH24_STENCIL8 });
    MSG::OGLFrameBufferCreateInfo info;
    info.defaultSize = { a_Size, 1 };
    info.colorBuffers.resize(OUTPUT_FRAG_DFD_COUNT);
    info.colorBuffers[OUTPUT_FRAG_DFD_GBUFFER0].attachment = GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_DFD_GBUFFER0;
    info.colorBuffers[OUTPUT_FRAG_DFD_GBUFFER1].attachment = GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_DFD_GBUFFER1;
    info.colorBuffers[OUTPUT_FRAG_DFD_VELOCITY].attachment = GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_DFD_VELOCITY;
    info.colorBuffers[OUTPUT_FRAG_DFD_FINAL].attachment    = GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_DFD_FINAL;
    info.colorBuffers[OUTPUT_FRAG_DFD_GBUFFER0].texture    = std::make_shared<MSG::OGLTexture2D>(a_Context, MSG::OGLTexture2DInfo { .width = a_Size.x, .height = a_Size.y, .levels = 1, .sizedFormat = GL_RGBA32UI });
    info.colorBuffers[OUTPUT_FRAG_DFD_GBUFFER1].texture    = std::make_shared<MSG::OGLTexture2D>(a_Context, MSG::OGLTexture2DInfo { .width = a_Size.x, .height = a_Size.y, .levels = 1, .sizedFormat = GL_RGBA32UI });
    info.colorBuffers[OUTPUT_FRAG_DFD_VELOCITY].texture    = std::make_shared<MSG::OGLTexture2D>(a_Context, MSG::OGLTexture2DInfo { .width = a_Size.x, .height = a_Size.y, .levels = 1, .sizedFormat = GL_RG16F });
    info.colorBuffers[OUTPUT_FRAG_DFD_FINAL].texture       = std::make_shared<MSG::OGLTexture2D>(a_Context, MSG::OGLTexture2DInfo { .width = a_Size.x, .height = a_Size.y, .levels = 1, .sizedFormat = GL_RGBA16F });
    info.depthBuffer.texture                               = depthStencilTexture;
    info.stencilBuffer.texture                             = depthStencilTexture;
    return std::make_shared<MSG::OGLFrameBuffer>(a_Context, info);
}

MSG::Renderer::DfdOpaqueGeometry::DfdOpaqueGeometry(Renderer::Impl& a_Renderer)
    : RenderPassInterface(/* NO DEPENDENCIES */)
{
    Add<DfdSubPassSkybox>(a_Renderer);
    Add<DfdSubPassOpaqueGeometry>();
    Sort();
}

void MSG::Renderer::DfdOpaqueGeometry::Update(Renderer::Impl& a_Renderer, const RenderPassesLibrary& a_RenderPasses)
{
    auto& clearColor        = a_Renderer.activeScene->GetBackgroundColor();
    auto& renderBuffer      = *a_Renderer.activeRenderBuffer;
    auto renderBufferSize   = glm::uvec3(renderBuffer->width, renderBuffer->height, 1);
    glm::uvec3 internalSize = glm::uvec3(glm::vec2(renderBufferSize) * a_Renderer.settings.internalResolution, 1);
    auto fbSize             = output != nullptr ? output->info.defaultSize : glm::uvec3(0);
    if (fbSize != internalSize) {
        output = CreateFbGeometry(a_Renderer.context, internalSize);
        // FILL VIEWPORT STATES
        auto& info                        = renderPassInfo;
        info.name                         = "DfdOpaqueGeometry";
        info.viewportState.viewport       = internalSize;
        info.viewportState.scissorExtent  = internalSize;
        info.frameBufferState.framebuffer = output;
        info.frameBufferState.clear.colors.resize(OUTPUT_FRAG_DFD_COUNT);
        info.frameBufferState.clear.colors[OUTPUT_FRAG_DFD_GBUFFER0] = { OUTPUT_FRAG_DFD_GBUFFER0, { 0, 0, 0, 0 } };
        info.frameBufferState.clear.colors[OUTPUT_FRAG_DFD_GBUFFER1] = { OUTPUT_FRAG_DFD_GBUFFER1, { 0, 0, 0, 0 } };
        info.frameBufferState.clear.colors[OUTPUT_FRAG_DFD_VELOCITY] = { OUTPUT_FRAG_DFD_VELOCITY, { 0, 0, 0, 0 } };
        info.frameBufferState.clear.colors[OUTPUT_FRAG_DFD_FINAL]    = { OUTPUT_FRAG_DFD_FINAL, { clearColor.r, clearColor.g, clearColor.b } };
        info.frameBufferState.clear.depthStencil                     = 0xffffff00u;
        info.frameBufferState.drawBuffers                            = {
            GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_DFD_GBUFFER0,
            GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_DFD_GBUFFER1,
            GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_DFD_VELOCITY,
            GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_DFD_FINAL
        };
    }
    RenderPassInterface::Update(a_Renderer, a_RenderPasses);
}

void MSG::Renderer::DfdOpaqueGeometry::Render(Renderer::Impl& a_Renderer)
{
    auto& cmdBuffer = a_Renderer.renderCmdBuffer;
    cmdBuffer.PushCmd<OGLCmdPushRenderPass>(renderPassInfo);
    for (auto& subPass : modules)
        subPass->Render(a_Renderer);
    cmdBuffer.PushCmd<OGLCmdEndRenderPass>();
}