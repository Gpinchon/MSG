#include <MSG/Debug.hpp>
#include <MSG/OGLContext/GLX.hpp>
#include <MSG/OGLContext/X11.hpp>

#include <GL/glxew.h>
#include <cassert>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace GLX {
#define APIVersion(major, minor) (major * 100 + minor * 10)
constexpr auto GLMajor            = 4;
constexpr auto GLMinor            = 5;
constexpr int glxContextAttribs[] = {
    GLX_CONTEXT_MAJOR_VERSION_ARB, GLMajor,
    GLX_CONTEXT_MINOR_VERSION_ARB, GLMinor,
#ifndef NDEBUG
    GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_DEBUG_BIT_ARB,
#endif // NDEBUG
    GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
    None
};
constexpr int glxConfigAttribs[] = {
    GLX_X_RENDERABLE, True,
    GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
    GLX_RENDER_TYPE, GLX_RGBA_BIT,
    GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
    GLX_RED_SIZE, 8,
    GLX_GREEN_SIZE, 8,
    GLX_BLUE_SIZE, 8,
    GLX_ALPHA_SIZE, 8,
    GLX_DEPTH_SIZE, 0,
    GLX_STENCIL_SIZE, 0,
    GLX_DOUBLEBUFFER, True,
    None
};
constexpr int glxHeadlessConfigAttribs[] = { None };

void InitializeGL()
{
    static bool s_Initialized = false;
    if (s_Initialized)
        return;
    glewExperimental = true;
    MSGCheckErrorFatal(
        auto result = glewInit(); result != GLEW_OK,
        reinterpret_cast<const char*>(glewGetErrorString(result)));
    s_Initialized = true;
}

void InitializeGLX()
{
    static bool s_Initialized = false;
    if (s_Initialized)
        return;
    auto window  = X11::WindowWrapper(std::make_shared<X11::DisplayWrapper>());
    auto display = std::any_cast<Display*>(window.display->handle);
    auto screen  = DefaultScreen(display);

    int visualattribs[] = { GLX_RGBA, None };
    auto visualInfo     = glXChooseVisual(display, screen, visualattribs);
    MSGCheckErrorFatal(visualInfo == nullptr, "glXChooseVisual failed");

    auto context = glXCreateContext(display, visualInfo, nullptr, True);
    MSGCheckErrorFatal(context == nullptr, "glXCreateContext failed");

    GLX::MakeCurrent(window.display->handle, window.handle, context);
    MSGCheckErrorFatal(
        auto result = glxewInit(); result != GLEW_OK,
        reinterpret_cast<const char*>(glewGetErrorString(result)));

    InitializeGL();
    XFree(visualInfo);
    s_Initialized = true;
}

auto CreateGLContext(
    const std::any& a_Display,
    const ContextWrapper* a_SharedContext,
    const bool& a_SetPixelFormat)
{
    InitializeGLX();

    auto display           = std::any_cast<Display*>(a_Display);
    auto screen            = DefaultScreen(display);
    GLXFBConfig* fbConfigs = nullptr;
    if (a_SetPixelFormat) {
        int configNbr = 0;
        fbConfigs     = glXChooseFBConfig(display, screen, glxConfigAttribs, &configNbr);
    } else {
        int configNbr = 0;
        fbConfigs     = glXChooseFBConfig(display, screen, glxHeadlessConfigAttribs, &configNbr);
    }
    MSGCheckErrorFatal(fbConfigs == nullptr, "glXChooseFBConfig failed");
    auto sharedCtx = a_SharedContext != nullptr ? std::any_cast<GLXContext>(a_SharedContext->handle) : nullptr;
    auto context   = glXCreateContextAttribsARB(display, fbConfigs[0], sharedCtx, True, glxContextAttribs);
    MSGCheckErrorFatal(context == nullptr, "glXCreateContextAttribsARB failed");
    free(fbConfigs);
    return context;
}
}

GLX::ContextWrapper::ContextWrapper(
    const std::any& a_XDisplay,
    const std::any& a_XDrawable,
    const ContextWrapper* a_SharedContext, const bool& a_SetPixelFormat)
    : handleDisplay(a_XDisplay)
    , handleDrawable(a_XDrawable)
    , handle(CreateGLContext(a_XDisplay, a_SharedContext, a_SetPixelFormat))
{
}

GLX::ContextWrapper::~ContextWrapper()
{
    auto display = std::any_cast<Display*>(handleDisplay);
    auto context = std::any_cast<GLXContext>(handle);
    glXDestroyContext(display, context);
}

uint64_t GLX::GetID(const std::any& a_GLXContext)
{
    auto context = std::any_cast<GLXContext>(a_GLXContext);
    return glXGetContextIDEXT(context);
}

void GLX::SwapBuffers(const std::any& a_XDisplay)
{
    auto display  = std::any_cast<Display*>(a_XDisplay);
    auto drawable = glXGetCurrentDrawable();
    glXSwapBuffers(display, drawable);
}

void GLX::Release(const std::any& a_XDisplay)
{
    auto display = std::any_cast<Display*>(a_XDisplay);
    MSGCheckErrorWarning(glXMakeCurrent(display, None, nullptr) != True, "glXMakeCurrent failed")
}

void GLX::SwapInterval(const std::any& a_XDisplay, const int8_t& a_Interval)
{
    auto display  = std::any_cast<Display*>(a_XDisplay);
    auto drawable = glXGetCurrentDrawable();
    glXSwapIntervalEXT(display, drawable, a_Interval);
}

void GLX::MakeCurrent(const std::any& a_XDisplay, const std::any& a_XDrawable, const std::any& a_GLXContext)
{
    auto display  = std::any_cast<Display*>(a_XDisplay);
    auto drawable = std::any_cast<XID>(a_XDrawable);
    auto context  = std::any_cast<GLXContext>(a_GLXContext);
    MSGCheckErrorWarning(glXMakeCurrent(display, drawable, context) != True, "glXMakeCurrent failed");
}

void GLX::MakeCurrent(const std::any& a_XDisplay, const std::any& a_GLXContext)
{
    auto display  = std::any_cast<Display*>(a_XDisplay);
    auto drawable = 0; // useful for headless contexts
    auto context  = std::any_cast<GLXContext>(a_GLXContext);
    MSGCheckErrorWarning(glXMakeCurrent(display, drawable, context) != True, "glXMakeCurrent failed");
}