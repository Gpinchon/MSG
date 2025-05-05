#pragma once

#include <MSG/OGLPipeline.hpp>

namespace MSG {
struct OGLCmdBufferState;
}

namespace MSG {
class OGLCmdPushPipeline {
public:
    OGLCmdPushPipeline(const OGLComputePipelineInfo& a_Info);
    OGLCmdPushPipeline(const OGLGraphicsPipeline& a_Info);
    void operator()(OGLCmdBufferState& a_State) const;

private:
    OGLPipeline _pipeline;
};
}