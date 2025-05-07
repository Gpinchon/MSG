#pragma once

namespace MSG {
class OGLRenderPass;
class OGLPipeline;
}

namespace MSG {
struct OGLCmdBufferExecutionState {
    const OGLRenderPass* renderPass = nullptr;
    const OGLPipeline* pipeline     = nullptr;
};
}