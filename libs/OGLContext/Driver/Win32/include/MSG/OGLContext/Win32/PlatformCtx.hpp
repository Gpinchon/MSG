#pragma once

#include <MSG/OGLContext/Win32/WGL.hpp>

namespace MSG {
struct OGLContexCreatetInfo;
}

namespace Platform {
class Ctx : public WGL::HGLRCWrapper {
    using WGL::HGLRCWrapper::HGLRCWrapper;
};

class CtxHeadless : public Ctx {
public:
    CtxHeadless(const MSG::OGLContexCreatetInfo& a_Info);
};

class CtxNormal : public Ctx {
public:
    CtxNormal(const MSG::OGLContexCreatetInfo& a_Info);
};
}