#pragma once

namespace MSG {
struct OGLCmdBufferExecutionState;
}

namespace MSG {
class OGLCmdEndRenderPass {
public:
    void operator()(OGLCmdBufferExecutionState& a_State) const;
};
}