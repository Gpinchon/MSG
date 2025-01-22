#pragma once

#include <any>

namespace MSG::Renderer {
struct CreateContextInfo;
}

namespace Platform {
class Context {
protected:
    Context() = default;

public:
    virtual ~Context();
    std::any hdc;
    std::any hglrc;
};

class HeadlessContext : public Context {
public:
    HeadlessContext(const MSG::Renderer::CreateContextInfo& a_Info);
    ~HeadlessContext() override;
    std::any hwnd;
};

class NormalContext : public Context {
public:
    NormalContext(const MSG::Renderer::CreateContextInfo& a_Info);
};

uint64_t CtxGetID(const Context& a_Ctx);
void CtxMakeCurrent(const Context& a_Ctx);
void CtxSwapBuffers(const Context& a_Ctx);
void CtxRelease();
void CtxSetSwapInterval(const int8_t& a_Interval);
}
