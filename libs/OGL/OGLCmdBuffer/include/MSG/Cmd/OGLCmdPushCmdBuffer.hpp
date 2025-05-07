#pragma once

namespace MSG {
class OGLCmdBuffer;
struct OGLCmdBufferExecutionState;
}

namespace MSG {
class OGLCmdPushCmdBuffer {
public:
    OGLCmdPushCmdBuffer(OGLCmdBuffer& a_CmdBuffer);
    void operator()(OGLCmdBufferExecutionState&);

private:
    OGLCmdBuffer& _cmdBuffer;
};
}