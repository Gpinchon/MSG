#include <MSG/Cmd/OGLCmdMemoryBarrier.hpp>

#include <GL/glew.h>

void MSG::OGLCmdMemoryBarrier::operator()(OGLCmdBufferExecutionState&)
{
    if (_byRegion)
        glMemoryBarrierByRegion(_barrierType);
    else
        glMemoryBarrier(_barrierType);
}