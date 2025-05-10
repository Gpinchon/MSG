#pragma once

#include <memory>

namespace MSG {
class OGLTexture;
struct OGLCmdBufferExecutionState;
}

namespace MSG {
class OGLCmdGenerateMipmap {
public:
    OGLCmdGenerateMipmap(const std::shared_ptr<OGLTexture>& a_Txt)
        : _txt(a_Txt)
    {
    }
    void operator()(OGLCmdBufferExecutionState& a_State) const;
    const std::shared_ptr<OGLTexture>& _txt;
};
}