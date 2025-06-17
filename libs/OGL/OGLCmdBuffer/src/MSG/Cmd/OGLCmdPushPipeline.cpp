#include <MSG/Cmd/OGLCmdPushPipeline.hpp>
#include <MSG/OGLCmdBufferExecutionState.hpp>

MSG::OGLCmdPushPipeline::OGLCmdPushPipeline(const OGLComputePipelineInfo& a_Info)
    : _pipeline(a_Info)
{
}

MSG::OGLCmdPushPipeline::OGLCmdPushPipeline(const OGLGraphicsPipeline& a_Info)
    : _pipeline(a_Info)
{
}

void MSG::OGLCmdPushPipeline::operator()(OGLCmdBufferExecutionState& a_State) const
{
    std::visit([&state = a_State](const auto& a_Pipeline) {
        OGLPipeline* prev = state.pipeline.has_value() ? &*state.pipeline : nullptr;
        a_Pipeline.Bind(prev);
    },
        _pipeline);
    a_State.pipeline.emplace(_pipeline);
}
