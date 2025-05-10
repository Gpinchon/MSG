#include <MSG/Cmd/OGLCmdPushRenderPass.hpp>
#include <MSG/OGLCmdBufferExecutionState.hpp>

#include <MSG/Debug.hpp>

MSG::OGLCmdPushRenderPass::OGLCmdPushRenderPass(const OGLRenderPassInfo& a_Info)
    : _renderPass(a_Info)
{
}

void MSG::OGLCmdPushRenderPass::operator()(OGLCmdBufferExecutionState& a_State) const
{
    checkErrorFatal(a_State.renderPass != nullptr, "CmdEndRenderPass not called");
    a_State.renderPass = &_renderPass;
    _renderPass.Begin();
}