#pragma once

#include <MSG/OGLPipeline.hpp>

namespace Msg {
struct OGLCmdBufferExecutionState;
}

namespace Msg {
class OGLCmdPushPipeline {
public:
    OGLCmdPushPipeline(const OGLComputePipelineInfo& a_Info);
    OGLCmdPushPipeline(const OGLGraphicsPipeline& a_Info);
    void operator()(OGLCmdBufferExecutionState& a_State) const;

private:
    OGLPipeline _pipeline;
};
}