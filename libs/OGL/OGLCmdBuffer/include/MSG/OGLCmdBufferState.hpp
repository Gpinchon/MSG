#pragma once

namespace MSG {
class OGLRenderPass;
class OGLPipeline;
}

namespace MSG {
struct OGLCmdBufferState {
    const OGLRenderPass* renderPass = nullptr;
    const OGLPipeline* pipeline     = nullptr;
};
}