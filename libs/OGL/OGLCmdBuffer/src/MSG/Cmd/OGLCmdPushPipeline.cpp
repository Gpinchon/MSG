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
        a_Pipeline.Bind(state.pipeline);
    },
        _pipeline);
    a_State.pipeline = &_pipeline;
}
