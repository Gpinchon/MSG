#pragma once

#include <memory>

namespace MSG {
class OGLCmdBuffer;
class OGLFrameBuffer;
}

namespace MSG::Renderer {
class SubsystemLibrary;
}

namespace MSG::Renderer {
class PassInterface {
public:
    virtual void Update(const SubsystemLibrary& a_Subsystems, PassInterface* a_Prev) = 0;
    virtual void Render(OGLCmdBuffer& a_CmdBuffer)                                   = 0;
    std::shared_ptr<OGLFrameBuffer> output;
};
}