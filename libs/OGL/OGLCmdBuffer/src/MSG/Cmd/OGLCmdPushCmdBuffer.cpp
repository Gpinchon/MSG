#include <MSG/Cmd/OGLCmdPushCmdBuffer.hpp>

#include <MSG/OGLCmdBuffer.hpp>

MSG::OGLCmdPushCmdBuffer::OGLCmdPushCmdBuffer(OGLCmdBuffer& a_CmdBuffer)
    : _cmdBuffer(a_CmdBuffer)
{
    OGLCmdBufferStatus expected = OGLCmdBufferStatus::Ready;
    OGLCmdBufferStatus desired  = OGLCmdBufferStatus::Pending;
    if (!_cmdBuffer._status.compare_exchange_strong(expected, desired))
        abort();
}

void MSG::OGLCmdPushCmdBuffer::operator()(OGLCmdBufferState& a_State)
{
    _cmdBuffer._ExecuteSub(a_State);
}