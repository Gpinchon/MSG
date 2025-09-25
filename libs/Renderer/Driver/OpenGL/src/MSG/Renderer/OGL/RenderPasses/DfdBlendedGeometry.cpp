#include <MSG/Renderer/OGL/RenderPasses/DfdBlendedGeometry.hpp>

#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLTexture2D.hpp>
#include <MSG/OGLTexture3D.hpp>
#include <MSG/Renderer/OGL/RenderBuffer.hpp>
#include <MSG/Renderer/OGL/RenderPasses/DfdLight.hpp>
#include <MSG/Renderer/OGL/RenderPasses/DfdOpaqueGeometry.hpp>
#include <MSG/Renderer/OGL/RenderPasses/DfdSSAO.hpp>
#include <MSG/Renderer/OGL/RenderPasses/DfdSubPassOITCompositing.hpp>
#include <MSG/Renderer/OGL/RenderPasses/DfdSubPassOITForward.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/Subsystems/MeshSubsystem.hpp>

#include <Bindings.glsl>
#include <OIT.glsl>

MSG::Renderer::DfdBlendedGeometry::DfdBlendedGeometry(Renderer::Impl& a_Renderer)
    : RenderPassInterface({ typeid(DfdOpaqueGeometry), typeid(DfdLight), typeid(DfdSSAO) })
{
    Add<DfdSubPassOITForward>();
    Add<DfdSubPassOITCompositing>(a_Renderer);
}

void MSG::Renderer::DfdBlendedGeometry::Update(Renderer::Impl& a_Renderer, const RenderPassesLibrary& a_RenderPasses)
{
    auto& geometryPass      = a_RenderPasses.Get<DfdOpaqueGeometry>();
    auto& fbGeometry        = geometryPass.output;
    auto& renderBuffer      = *a_Renderer.activeRenderBuffer;
    auto renderBufferSize   = glm::uvec3(renderBuffer->width, renderBuffer->height, 1);
    glm::uvec3 internalSize = glm::uvec3(glm::vec2(renderBufferSize) * a_Renderer.internalResolution, 1);
    auto fbSize             = output != nullptr ? output->info.defaultSize : glm::uvec3(0);
    if (fbSize != internalSize) {
        OGLFrameBufferCreateInfo fbInfo;
        fbInfo.colorBuffers.resize(1);
        fbInfo.defaultSize                          = internalSize;
        fbInfo.colorBuffers[0].attachment           = GL_COLOR_ATTACHMENT0;
        fbInfo.colorBuffers[0].texture              = fbGeometry->info.colorBuffers[OUTPUT_FRAG_DFD_FINAL].texture;
        fbInfo.depthBuffer                          = fbGeometry->info.depthBuffer;
        output                                      = std::make_shared<OGLFrameBuffer>(a_Renderer.context, fbInfo);
        renderPassInfo.name                         = "OIT";
        renderPassInfo.viewportState.viewport       = internalSize;
        renderPassInfo.viewportState.scissorExtent  = internalSize;
        renderPassInfo.frameBufferState.framebuffer = output;
        renderPassInfo.frameBufferState.drawBuffers = { GL_COLOR_ATTACHMENT0 };
    }
    RenderPassInterface::Update(a_Renderer, a_RenderPasses);
}

void MSG::Renderer::DfdBlendedGeometry::Render(Impl& a_Renderer)
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