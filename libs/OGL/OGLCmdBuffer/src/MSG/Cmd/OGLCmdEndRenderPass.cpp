#include <MSG/Cmd/OGLCmdEndRenderPass.hpp>
#include <MSG/Debug.hpp>
#include <MSG/OGLCmdBufferExecutionState.hpp>
#include <MSG/OGLRenderPass.hpp>

void Msg::OGLCmdEndRenderPass::operator()(OGLCmdBufferExecutionState& a_State) const
{
    MSGCheckErrorFatal(a_State.renderPass == nullptr, "CmdBegin/PushRenderPass was not called!");
    a_State.renderPass->End();
    a_State.renderPass = nullptr;
}
