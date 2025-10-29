#include <MSG/Renderer/OGL/RenderPasses/PassPostTreatment.hpp>

#include <MSG/Renderer/OGL/RenderPasses/PassLight.hpp>
#include <MSG/Renderer/OGL/RenderPasses/PassOpaqueGeometry.hpp>
#include <MSG/Renderer/OGL/RenderPasses/SubPassFog.hpp>
#include <MSG/Renderer/OGL/RenderPasses/SubPassSSAO.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>

Msg::Renderer::PassPostTreatment::PassPostTreatment(Renderer::Impl& a_Renderer)
    : RenderPassInterface({ typeid(PassOpaqueGeometry), typeid(PassLight) })
{
    Add<SubPassSSAO>(a_Renderer);
    Add<SubPassFog>(a_Renderer);
}

void Msg::Renderer::PassPostTreatment::Update(Renderer::Impl& a_Renderer, const RenderPassesLibrary& a_RenderPasses)
{
    renderPassInfo      = a_RenderPasses.Get<PassLight>().renderPassInfo;
    renderPassInfo.name = "PostTreatment";
    RenderPassInterface::Update(a_Renderer, a_RenderPasses);
}

void Msg::Renderer::PassPostTreatment::Render(Impl& a_Renderer)
{
    auto& cmdBuffer = a_Renderer.renderCmdBuffer;

    cmdBuffer.PushCmd<OGLCmdPushRenderPass>(renderPassInfo);
    for (auto& subPass : modules)
        subPass->Render(a_Renderer);
    cmdBuffer.PushCmd<OGLCmdEndRenderPass>();
}