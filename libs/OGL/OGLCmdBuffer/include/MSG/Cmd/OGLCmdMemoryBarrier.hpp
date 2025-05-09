#pragma once

namespace MSG {
struct OGLCmdBufferExecutionState;
}

namespace MSG {
class OGLCmdMemoryBarrier {
public:
    OGLCmdMemoryBarrier(const unsigned& a_BarrierType, const bool& a_ByRegion = false)
        : _barrierType(a_BarrierType)
        , _byRegion(a_ByRegion)
    {
    }
    void operator()(OGLCmdBufferExecutionState&);

private:
    const unsigned _barrierType;
    const bool _byRegion;
};
}