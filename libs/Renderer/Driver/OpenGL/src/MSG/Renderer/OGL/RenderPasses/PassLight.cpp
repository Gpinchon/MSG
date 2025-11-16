#include <MSG/Renderer/OGL/RenderPasses/PassLight.hpp>

#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLTexture2D.hpp>
#include <MSG/Renderer/OGL/RenderBuffer.hpp>
#include <MSG/Renderer/OGL/RenderPasses/PassOpaqueGeometry.hpp>
#include <MSG/Renderer/OGL/RenderPasses/SubPassIBL.hpp>
#include <MSG/Renderer/OGL/RenderPasses/SubPassShadow.hpp>
#include <MSG/Renderer/OGL/RenderPasses/SubPassVTFS.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/Subsystems/MeshSubsystem.hpp>

#include <Bindings.glsl>

Msg::Renderer::PassLight::PassLight(Renderer::Impl& a_Renderer)
    : RenderPassInterface({ typeid(PassOpaqueGeometry) })
{
    Add<SubPassIBL>(a_Renderer);
    Add<SubPassShadow>(a_Renderer);
    Add<SubPassVTFS>(a_Renderer);
    Sort();
}

void Msg::Renderer::PassLight::Update(Renderer::Impl& a_Renderer, const RenderPassesLibrary& a_RenderPasses)
{
    auto& geometryPass      = a_RenderPasses.Get<PassOpaqueGeometry>();
    auto& fbGeometry        = geometryPass.output;
    auto& renderBuffer      = *a_Renderer.activeRenderBuffer;
    auto renderBufferSize   = glm::uvec3(renderBuffer->width, renderBuffer->height, 1);
    glm::uvec3 internalSize = glm::uvec3(glm::vec2(renderBufferSize) * a_Renderer.settings.internalResolution, 1);
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

void Msg::Renderer::PassLight::Render(Impl& a_Renderer)
{
    auto& activeScene = *a_Renderer.activeScene;
    auto& cmdBuffer   = a_Renderer.renderCmdBuffer;

    cmdBuffer.PushCmd<OGLCmdPushRenderPass>(renderPassInfo);
    for (auto& subPass : modules)
        subPass->Render(a_Renderer);
    cmdBuffer.PushCmd<OGLCmdEndRenderPass>();
}