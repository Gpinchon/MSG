#pragma once

#include <cstdint>
#include <functional>

namespace MSG {
struct OGLCmdBufferState;
}

namespace MSG {
struct OGLCmdDrawInfo {
    OGLCmdDrawInfo()
        : indexCount(0)
        , indexOffset(0)
    {
    }
    bool indexed            = false;
    uint32_t instanceCount  = 1;
    uint32_t instanceOffset = 0;
    uint32_t vertexOffset   = 0;
    union {
        struct {
            uint32_t indexCount;
            uint32_t indexOffset;
        };
        struct {
            uint32_t vertexCount;
        };
    };
};

class OGLCmdDraw {
public:
    OGLCmdDraw(const OGLCmdDrawInfo& a_Info);
    void operator()(OGLCmdBufferState& a_State) { _functor(a_State); }

private:
    const std::function<void(OGLCmdBufferState&)> _functor;
};
}