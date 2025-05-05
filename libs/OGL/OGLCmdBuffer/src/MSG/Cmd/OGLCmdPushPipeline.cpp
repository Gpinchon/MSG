#include <MSG/Cmd/OGLCmdPushPipeline.hpp>
#include <MSG/OGLCmdBufferState.hpp>

MSG::OGLCmdPushPipeline::OGLCmdPushPipeline(const OGLComputePipelineInfo& a_Info)
    : _pipeline(a_Info)
{
}

MSG::OGLCmdPushPipeline::OGLCmdPushPipeline(const OGLGraphicsPipeline& a_Info)
    : _pipeline(a_Info)
{
}

void PushPipeline(const MSG::OGLGraphicsPipeline& a_GP, const MSG::OGLCmdBufferState& a_State)
{
    auto gp = std::get_if<MSG::OGLGraphicsPipeline>(a_State.pipeline);
    a_GP.Bind(gp);
}

void PushPipeline(const MSG::OGLComputePipeline& a_GP, const MSG::OGLCmdBufferState& a_State)
{
    auto gp = std::get_if<MSG::OGLComputePipeline>(a_State.pipeline);
    a_GP.Bind(gp);
}

void MSG::OGLCmdPushPipeline::operator()(OGLCmdBufferState& a_State) const
{
    std::visit([&state = a_State](const auto& a_Pipeline) {
        PushPipeline(a_Pipeline, state);
    },
        _pipeline);
    a_State.pipeline = &_pipeline;
}
