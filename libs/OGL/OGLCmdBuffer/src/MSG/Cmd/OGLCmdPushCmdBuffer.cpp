#include <MSG/Cmd/OGLCmdPushCmdBuffer.hpp>

#include <MSG/OGLCmdBuffer.hpp>

Msg::OGLCmdPushCmdBuffer::OGLCmdPushCmdBuffer(OGLCmdBuffer& a_CmdBuffer)
    : _cmdBuffer(a_CmdBuffer)
{
    _cmdBuffer._ChangeState(OGLCmdBufferState::Ready, OGLCmdBufferState::Pending);
}

void Msg::OGLCmdPushCmdBuffer::operator()(OGLCmdBufferExecutionState& a_State)
{
    _cmdBuffer._ExecuteSub(a_State);
}