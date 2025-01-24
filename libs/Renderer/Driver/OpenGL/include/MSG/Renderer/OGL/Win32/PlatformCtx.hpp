#pragma once

#include <MSG/Renderer/OGL/Win32/WGL.hpp>
#include <MSG/Renderer/OGL/Win32/Win32.hpp>

namespace MSG::Renderer {
struct CreateContextInfo;
}

namespace Platform {
class Ctx : public WGL::HGLRCWrapper {
    using WGL::HGLRCWrapper::HGLRCWrapper;
};

class CtxHeadless : public Ctx {
public:
    CtxHeadless(const MSG::Renderer::CreateContextInfo& a_Info);
};

class CtxNormal : public Ctx {
public:
    CtxNormal(const MSG::Renderer::CreateContextInfo& a_Info);
};
}