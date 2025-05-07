#pragma once

#include <MSG/OGLRenderPass.hpp>

namespace MSG {
struct OGLCmdBufferExecutionState;
}

namespace MSG {
class OGLCmdPushRenderPass {
public:
    OGLCmdPushRenderPass(const OGLRenderPassInfo& a_Info);
    void operator()(OGLCmdBufferExecutionState& a_State) const;

private:
    OGLRenderPass _renderPass;
};
}