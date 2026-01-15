#include <MSG/Renderer/OGL/RenderPasses/PassBlendedGeometry.hpp>

#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLTexture2D.hpp>
#include <MSG/OGLTexture3D.hpp>
#include <MSG/Renderer/OGL/RenderBuffer.hpp>
#include <MSG/Renderer/OGL/RenderPasses/PassLight.hpp>
#include <MSG/Renderer/OGL/RenderPasses/PassOpaqueGeometry.hpp>
#include <MSG/Renderer/OGL/RenderPasses/PassPostTreatment.hpp>
#include <MSG/Renderer/OGL/RenderPasses/SubPassOITCompositing.hpp>
#include <MSG/Renderer/OGL/RenderPasses/SubPassOITForward.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/Subsystems/MeshSubsystem.hpp>

#include <Bindings.glsl>

Msg::Renderer::PassBlendedGeometry::PassBlendedGeometry(Renderer::Impl& a_Renderer)
    : RenderPassInterface({ typeid(PassOpaqueGeometry), typeid(PassLight), typeid(PassPostTreatment) })
{
    Add<SubPassOITForward>();
    Add<SubPassOITCompositing>(a_Renderer);
}

void Msg::Renderer::PassBlendedGeometry::Update(Renderer::Impl& a_Renderer, const RenderPassesLibrary& a_RenderPasses)
{
    auto& geometryPass      = a_RenderPasses.Get<PassOpaqueGeometry>();
    auto& fbGeometry        = geometryPass.output;
    auto& renderBuffer      = *a_Renderer.activeRenderBuffer;
    auto renderBufferSize   = glm::uvec3(renderBuffer->width, renderBuffer->height, 1);
    glm::uvec3 internalSize = glm::uvec3(glm::vec2(renderBufferSize) * a_Renderer.settings.internalResolution, 1);
    auto fbSize             = output != nullptr ? output->info.defaultSize : glm::uvec3(0);
    if (fbSize != internalSize) {
        OGLFrameBufferCreateInfo fbInfo;
        fbInfo.colorBuffers.resize(OUTPUT_FRAG_OIT_COUNT);
        fbInfo.defaultSize                                       = internalSize;
        fbInfo.colorBuffers[OUTPUT_FRAG_OIT_COLOR].attachment    = GL_COLOR_ATTACHMENT0;
        fbInfo.colorBuffers[OUTPUT_FRAG_OIT_COLOR].texture       = fbGeometry->info.colorBuffers[OUTPUT_FRAG_DFD_FINAL].texture;
        fbInfo.colorBuffers[OUTPUT_FRAG_OIT_VELOCITY].attachment = GL_COLOR_ATTACHMENT1;
        fbInfo.colorBuffers[OUTPUT_FRAG_OIT_VELOCITY].texture    = fbGeometry->info.colorBuffers[OUTPUT_FRAG_DFD_VELOCITY].texture;
        fbInfo.depthBuffer                                       = fbGeometry->info.depthBuffer;
        output                                                   = std::make_shared<OGLFrameBuffer>(a_Renderer.context, fbInfo);
        renderPassInfo.name                                      = "OIT";
        renderPassInfo.viewportState.viewportExtent              = internalSize;
        renderPassInfo.viewportState.scissorExtent               = internalSize;
        renderPassInfo.frameBufferState.framebuffer              = output;
        renderPassInfo.frameBufferState.drawBuffers              = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    }
    RenderPassInterface::Update(a_Renderer, a_RenderPasses);
}

void Msg::Renderer::PassBlendedGeometry::Render(Impl& a_Renderer)
{
    auto& meshSubsystem = a_Renderer.subsystemsLibrary.Get<MeshSubsystem>();
    auto& cmdBuffer     = a_Renderer.renderCmdBuffer;

    if (meshSubsystem.blended.empty())
        return;
    cmdBuffer.PushCmd<OGLCmdPushRenderPass>(renderPassInfo);
    for (auto& renderPass : modules)
        renderPass->Render(a_Renderer);
    cmdBuffer.PushCmd<OGLCmdEndRenderPass>();
}