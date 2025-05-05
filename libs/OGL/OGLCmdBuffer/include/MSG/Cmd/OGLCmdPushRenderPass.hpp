#pragma once

#include <MSG/OGLRenderPass.hpp>

namespace MSG {
struct OGLCmdBufferState;
}

namespace MSG {
class OGLCmdPushRenderPass {
public:
    OGLCmdPushRenderPass(const OGLRenderPassInfo& a_Info);
    void operator()(OGLCmdBufferState& a_State) const;

private:
    OGLRenderPass _renderPass;
};
}