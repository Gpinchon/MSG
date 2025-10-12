#pragma once

#include <MSG/OGLContext/Win32/WGL.hpp>

namespace Msg {
struct OGLContextCreateInfo;
}

namespace Platform {
class Ctx : public WGL::HGLRCWrapper {
    using WGL::HGLRCWrapper::HGLRCWrapper;
};

class CtxHeadless : public Ctx {
public:
    CtxHeadless(const Msg::OGLContextCreateInfo& a_Info);
};

class CtxNormal : public Ctx {
public:
    CtxNormal(const Msg::OGLContextCreateInfo& a_Info);
};
}