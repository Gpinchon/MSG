#pragma once

#include <memory>
#include <variant>

namespace Msg {
class OGLBuffer;
struct OGLCmdBufferExecutionState;
}

namespace Msg {
struct OGLCopyBufferInfo {
    size_t readOffset  = 0;
    size_t writeOffset = 0;
    size_t size        = 0;
};
class OGLCmdCopyBuffer {
public:
    OGLCmdCopyBuffer(
        const std::shared_ptr<OGLBuffer>& a_ReadBuffer,
        const std::shared_ptr<OGLBuffer>& a_WriteBuffer,
        const OGLCopyBufferInfo& a_Info);
    void operator()(OGLCmdBufferExecutionState&) const;

private:
    const std::shared_ptr<OGLBuffer> _readBuffer;
    const std::shared_ptr<OGLBuffer> _writeBuffer;
    const OGLCopyBufferInfo _info;
};
}
