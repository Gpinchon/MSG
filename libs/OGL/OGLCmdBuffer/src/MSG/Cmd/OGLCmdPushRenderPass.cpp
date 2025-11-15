#include <MSG/Cmd/OGLCmdPushRenderPass.hpp>
#include <MSG/OGLCmdBufferExecutionState.hpp>
#include <MSG/OGLRenderPass.hpp>

#include <MSG/Debug.hpp>

Msg::OGLCmdPushRenderPass::OGLCmdPushRenderPass(const OGLRenderPassInfo& a_Info)
    : _renderPass(std::make_shared<OGLRenderPass>(a_Info))
{
}

void Msg::OGLCmdPushRenderPass::operator()(OGLCmdBufferExecutionState& a_State) const
{
    MSGCheckErrorFatal(a_State.renderPass != nullptr, "CmdEndRenderPass not called");
    a_State.renderPass = _renderPass;
    _renderPass->Begin();
}
