#pragma once

#include <MSG/OGLRenderPass.hpp>

namespace Msg {
struct OGLCmdBufferExecutionState;
}

namespace Msg {
class OGLCmdPushRenderPass {
public:
    OGLCmdPushRenderPass(const OGLRenderPassInfo& a_Info);
    void operator()(OGLCmdBufferExecutionState& a_State) const;

private:
    OGLRenderPass _renderPass;
};
}