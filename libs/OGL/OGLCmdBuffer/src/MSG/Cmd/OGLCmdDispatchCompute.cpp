#include <MSG/Cmd/OGLCmdDispatchCompute.hpp>
#include <MSG/OGLCmdBufferExecutionState.hpp>

#include <GL/glew.h>

Msg::OGLCmdDispatchCompute::OGLCmdDispatchCompute(const OGLCmdDispatchComputeInfo& a_Info)
    : _info(a_Info)
{
}

void Msg::OGLCmdDispatchCompute::operator()(OGLCmdBufferExecutionState& a_State)
{
    auto& cp = std::get<OGLComputePipeline>(*a_State.pipeline);
    glDispatchCompute(_info.workgroupX, _info.workgroupY, _info.workgroupZ);
}