#pragma once

namespace Msg {
struct OGLCmdBufferExecutionState;
}

namespace Msg {
class OGLCmdEndRenderPass {
public:
    void operator()(OGLCmdBufferExecutionState& a_State) const;
};
}