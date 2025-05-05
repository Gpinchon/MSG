#include <MSG/Cmd/OGLCmdDispatchCompute.hpp>
#include <MSG/OGLCmdBufferState.hpp>
#include <MSG/OGLPipeline.hpp>

#include <GL/glew.h>

MSG::OGLCmdDispatchCompute::OGLCmdDispatchCompute(const OGLCmdDispatchComputeInfo& a_Info)
    : _info(a_Info)
{
}

void MSG::OGLCmdDispatchCompute::operator()(OGLCmdBufferState& a_State)
{
    auto& cp = std::get<OGLComputePipeline>(*a_State.pipeline);
    glDispatchCompute(_info.workgroupX, _info.workgroupY, _info.workgroupZ);
    if (cp.memoryBarrier != GL_NONE)
        glMemoryBarrier(cp.memoryBarrier);
}