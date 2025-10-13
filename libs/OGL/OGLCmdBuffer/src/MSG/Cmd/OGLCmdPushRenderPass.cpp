#include <MSG/Cmd/OGLCmdPushRenderPass.hpp>
#include <MSG/OGLCmdBufferExecutionState.hpp>

#include <MSG/Debug.hpp>

Msg::OGLCmdPushRenderPass::OGLCmdPushRenderPass(const OGLRenderPassInfo& a_Info)
    : _renderPass(a_Info)
{
}

void Msg::OGLCmdPushRenderPass::operator()(OGLCmdBufferExecutionState& a_State) const
{
    MSGCheckErrorFatal(a_State.renderPass.has_value(), "CmdEndRenderPass not called");
    a_State.renderPass.emplace(_renderPass);
    _renderPass.Begin();
}