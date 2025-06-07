#include <MSG/Cmd/OGLCmdCopyBuffer.hpp>
#include <MSG/OGLBuffer.hpp>

#include <GL/glew.h>

MSG::OGLCmdCopyBuffer::OGLCmdCopyBuffer(
    const std::shared_ptr<OGLBuffer>& a_ReadBuffer,
    const std::shared_ptr<OGLBuffer>& a_WriteBuffer,
    const OGLCopyBufferInfo& a_Info)
    : _readBuffer(a_ReadBuffer)
    , _writeBuffer(a_WriteBuffer)
    , _info(a_Info)
{
}

void MSG::OGLCmdCopyBuffer::operator()(OGLCmdBufferExecutionState&) const
{
    glCopyNamedBufferSubData(
        *_readBuffer, *_writeBuffer,
        _info.readOffset, _info.writeOffset,
        _info.size);
}