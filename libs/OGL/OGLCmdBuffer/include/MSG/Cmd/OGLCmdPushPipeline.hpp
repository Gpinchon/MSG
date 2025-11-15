#pragma once

#include <memory>

namespace Msg {
class OGLPipeline;
struct OGLCmdBufferExecutionState;
struct OGLComputePipelineInfo;
struct OGLGraphicsPipelineInfo;
}

namespace Msg {
class OGLCmdPushPipeline {
public:
    OGLCmdPushPipeline(const OGLComputePipelineInfo& a_Info);
    OGLCmdPushPipeline(const OGLGraphicsPipelineInfo& a_Info);
    void operator()(OGLCmdBufferExecutionState& a_State) const;

private:
    std::shared_ptr<OGLPipeline> _pipeline;
};
}
