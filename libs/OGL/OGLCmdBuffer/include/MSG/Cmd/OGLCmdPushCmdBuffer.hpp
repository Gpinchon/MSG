#pragma once

namespace Msg {
class OGLCmdBuffer;
struct OGLCmdBufferExecutionState;
}

namespace Msg {
class OGLCmdPushCmdBuffer {
public:
    OGLCmdPushCmdBuffer(OGLCmdBuffer& a_CmdBuffer);
    void operator()(OGLCmdBufferExecutionState&);

private:
    OGLCmdBuffer& _cmdBuffer;
};
}