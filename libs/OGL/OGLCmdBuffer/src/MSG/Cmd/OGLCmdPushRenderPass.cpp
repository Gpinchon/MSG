#include <MSG/Cmd/OGLCmdPushRenderPass.hpp>
#include <MSG/OGLCmdBufferState.hpp>

MSG::OGLCmdPushRenderPass::OGLCmdPushRenderPass(const OGLRenderPassInfo& a_Info)
    : _renderPass(a_Info)
{
}

void MSG::OGLCmdPushRenderPass::operator()(OGLCmdBufferState& a_State) const
{
    a_State.renderPass = &_renderPass;
    _renderPass.Begin();
}