#pragma once

#include <memory>

namespace Msg {
class OGLRenderPass;
struct OGLRenderPassInfo;
struct OGLCmdBufferExecutionState;
}

namespace Msg {
class OGLCmdPushRenderPass {
public:
    OGLCmdPushRenderPass(const OGLRenderPassInfo& a_Info);
    void operator()(OGLCmdBufferExecutionState& a_State) const;

private:
    std::shared_ptr<OGLRenderPass> _renderPass;
};
}
