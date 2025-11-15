#include <MSG/Cmd/OGLCmdPushPipeline.hpp>
#include <MSG/OGLCmdBufferExecutionState.hpp>
#include <MSG/OGLPipeline.hpp>

Msg::OGLCmdPushPipeline::OGLCmdPushPipeline(const OGLComputePipelineInfo& a_Info)
    : _pipeline(std::make_shared<OGLPipeline>(a_Info))
{
}

Msg::OGLCmdPushPipeline::OGLCmdPushPipeline(const OGLGraphicsPipelineInfo& a_Info)
    : _pipeline(std::make_shared<OGLPipeline>(a_Info))
{
}

void Msg::OGLCmdPushPipeline::operator()(OGLCmdBufferExecutionState& a_State) const
{
    std::visit([&state = a_State](const auto& a_Pipeline) {
        a_Pipeline.Bind(state.pipeline.get());
    },
        *_pipeline);
    a_State.pipeline = _pipeline;
}
