#include <MSG/Renderer/OGL/RenderPasses/DfdLight.hpp>

#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLTexture2D.hpp>
#include <MSG/Renderer/OGL/RenderBuffer.hpp>
#include <MSG/Renderer/OGL/RenderPasses/DfdOpaqueGeometry.hpp>
#include <MSG/Renderer/OGL/RenderPasses/DfdSubPassIBL.hpp>
#include <MSG/Renderer/OGL/RenderPasses/DfdSubPassShadow.hpp>
#include <MSG/Renderer/OGL/RenderPasses/DfdSubPassVTFS.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/Subsystems/MeshSubsystem.hpp>

#include <Bindings.glsl>

MSG::Renderer::DfdLight::DfdLight(Renderer::Impl& a_Renderer)
    : RenderPassInterface({ typeid(DfdOpaqueGeometry) })
{
    Add<DfdSubPassIBL>(a_Renderer);
    Add<DfdSubPassShadow>();
    Add<DfdSubPassVTFS>(a_Renderer);
    Sort();
}

void MSG::Renderer::DfdLight::Update(Renderer::Impl& a_Renderer, const RenderPassesLibrary& a_RenderPasses)
{
    auto& geometryPass      = a_RenderPasses.Get<DfdOpaqueGeometry>();
    auto& fbGeometry        = geometryPass.output;
    auto& renderBuffer      = *a_Renderer.activeRenderBuffer;
    auto renderBufferSize   = glm::uvec3(renderBuffer->width, renderBuffer->height, 1);
    glm::uvec3 internalSize = glm::uvec3(glm::vec2(renderBufferSize) * a_Renderer.internalResolution, 1);
    auto fbSize             = output != nullptr ? output->info.defaultSize : glm::uvec3(0);
    if (fbSize != internalSize) {
        OGLFrameBufferCreateInfo fbInfo;
        fbInfo.defaultSize = internalSize;
        fbInfo.colorBuffers.resize(1);
        fbInfo.colorBuffers[0].attachment = GL_COLOR_ATTACHMENT0;
        fbInfo.colorBuffers[0].texture    = fbGeometry->info.colorBuffers[OUTPUT_FRAG_DFD_FINAL].texture;
        fbInfo.stencilBuffer              = fbGeometry->info.stencilBuffer;
        output                            = std::make_shared<OGLFrameBuffer>(a_Renderer.context, fbInfo);
        // FILL VIEWPORT STATES
        auto& info                        = renderPassInfo;
        info.name                         = "Lighting";
        info.viewportState.viewport       = internalSize;
        info.viewportState.scissorExtent  = internalSize;
        info.frameBufferState.framebuffer = output;
        info.frameBufferState.drawBuffers = {
            GL_COLOR_ATTACHMENT0
        };
    }
    RenderPassInterface::Update(a_Renderer, a_RenderPasses);
}

// TODO split this chunk
void MSG::Renderer::DfdLight::Render(Impl& a_Renderer)
{
    auto& activeScene = *a_Renderer.activeScene;
    auto& cmdBuffer   = a_Renderer.renderCmdBuffer;

    cmdBuffer.PushCmd<OGLCmdPushRenderPass>(renderPassInfo);
    for (auto& subPass : modules)
        subPass->Render(a_Renderer);
    cmdBuffer.PushCmd<OGLCmdEndRenderPass>();
}