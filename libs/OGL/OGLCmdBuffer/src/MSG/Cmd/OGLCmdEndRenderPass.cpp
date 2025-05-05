#include <MSG/Cmd/OGLCmdEndRenderPass.hpp>
#include <MSG/OGLCmdBufferState.hpp>
#include <MSG/OGLPipeline.hpp>
#include <MSG/OGLRenderPass.hpp>

void MSG::OGLCmdEndRenderPass::operator()(OGLCmdBufferState& a_State) const
{
    std::visit(
        [](auto& a_Pipeline) { a_Pipeline.Restore(); },
        *a_State.pipeline);
    a_State.pipeline = nullptr;
    a_State.renderPass->End();
    a_State.renderPass = nullptr;
}