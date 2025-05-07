#pragma once

#include <cstdint>
#include <functional>

namespace MSG {
struct OGLCmdBufferExecutionState;
}

namespace MSG {
struct OGLCmdDispatchComputeInfo {
    uint16_t workgroupX = 1;
    uint16_t workgroupY = 1;
    uint16_t workgroupZ = 1;
};

class OGLCmdDispatchCompute {
public:
    OGLCmdDispatchCompute(const OGLCmdDispatchComputeInfo& a_Info);
    void operator()(OGLCmdBufferExecutionState& a_State);

private:
    const OGLCmdDispatchComputeInfo _info;
};
}