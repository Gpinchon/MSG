#pragma once

#include <MSG/OGLContext/GLX.hpp>

namespace MSG {
struct OGLContextCreateInfo;
}

namespace Platform {
class Ctx : public GLX::ContextWrapper {
    using GLX::ContextWrapper::ContextWrapper;
};

// Headless context creates its own Display
class CtxHeadless : public Ctx {
public:
    CtxHeadless(const MSG::OGLContextCreateInfo& a_Info);
    ~CtxHeadless();
};

class CtxNormal : public Ctx {
public:
    CtxNormal(const MSG::OGLContextCreateInfo& a_Info);
};
}