#include <MSG/Cmd/OGLCmdPushPipeline.hpp>
#include <MSG/OGLCmdBufferExecutionState.hpp>

Msg::OGLCmdPushPipeline::OGLCmdPushPipeline(const OGLComputePipelineInfo& a_Info)
    : _pipeline(a_Info)
{
}

Msg::OGLCmdPushPipeline::OGLCmdPushPipeline(const OGLGraphicsPipeline& a_Info)
    : _pipeline(a_Info)
{
}

void Msg::OGLCmdPushPipeline::operator()(OGLCmdBufferExecutionState& a_State) const
{
    std::visit([&state = a_State](const auto& a_Pipeline) {
        const OGLPipeline* prev = state.pipeline.has_value() ? std::to_address(state.pipeline) : nullptr;
        a_Pipeline.Bind(prev);
    },
        _pipeline);
    a_State.pipeline.emplace(_pipeline);
}
