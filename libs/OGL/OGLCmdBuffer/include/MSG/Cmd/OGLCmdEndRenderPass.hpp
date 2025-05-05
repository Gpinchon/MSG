#pragma once

namespace MSG {
struct OGLCmdBufferState;
}

namespace MSG {
class OGLCmdEndRenderPass {
public:
    void operator()(OGLCmdBufferState& a_State) const;
};
}