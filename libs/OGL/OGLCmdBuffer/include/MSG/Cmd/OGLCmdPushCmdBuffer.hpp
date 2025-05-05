#pragma once

namespace MSG {
class OGLCmdBuffer;
struct OGLCmdBufferState;
}

namespace MSG {
class OGLCmdPushCmdBuffer {
public:
    OGLCmdPushCmdBuffer(OGLCmdBuffer& a_CmdBuffer);
    void operator()(OGLCmdBufferState&);

private:
    OGLCmdBuffer& _cmdBuffer;
};
}