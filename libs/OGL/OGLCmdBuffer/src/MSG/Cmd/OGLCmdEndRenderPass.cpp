#include <MSG/Cmd/OGLCmdEndRenderPass.hpp>
#include <MSG/OGLCmdBufferExecutionState.hpp>
#include <MSG/OGLRenderPass.hpp>

void MSG::OGLCmdEndRenderPass::operator()(OGLCmdBufferExecutionState& a_State) const
{
    a_State.renderPass->End(); // we should crash here if CmdBegin/PushRenderPass was not called
    a_State.renderPass = nullptr;
}