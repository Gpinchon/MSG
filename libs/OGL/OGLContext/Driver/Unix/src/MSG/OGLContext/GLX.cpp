#include <MSG/Debug.hpp>
#include <MSG/OGLContext/GLX.hpp>
#include <MSG/OGLContext/X11.hpp>

#include <GL/glxew.h>
#include <X11/Xlib.h>
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
#ifdef MSG_DEBUG
    GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_DEBUG_BIT_ARB,
#endif // MSG_DEBUG
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

GLXFBConfig SelectFBConfigForWindow(Display* a_Display, int a_Screen, Window a_Window)
{
    XWindowAttributes attrs;
    MSGCheckErrorFatal(!XGetWindowAttributes(a_Display, a_Window, &attrs), "XGetWindowAttributes failed");
    const auto targetVisualId = XVisualIDFromVisual(attrs.visual);

    int configCount         = 0;
    GLXFBConfig* allConfigs = glXGetFBConfigs(a_Display, a_Screen, &configCount);
    MSGCheckErrorFatal(allConfigs == nullptr, "glXGetFBConfigs failed");

    GLXFBConfig match = nullptr;
    for (int i = 0; i < configCount; ++i) {
        int visualId = 0;
        glXGetFBConfigAttrib(a_Display, allConfigs[i], GLX_VISUAL_ID, &visualId);
        if (VisualID(visualId) == targetVisualId) {
            match = allConfigs[i];
            break;
        }
    }
    XFree(allConfigs);
    MSGCheckErrorFatal(match == nullptr, "No GLXFBConfig matches the target window's visual");
    return match;
}

GLXFBConfig SelectFBConfig(Display* a_Display, int a_Screen, const bool& a_SetPixelFormat)
{
    const int* attribs     = a_SetPixelFormat ? glxConfigAttribs : glxHeadlessConfigAttribs;
    int configNbr          = 0;
    GLXFBConfig* fbConfigs = glXChooseFBConfig(a_Display, a_Screen, attribs, &configNbr);
    MSGCheckErrorFatal(fbConfigs == nullptr, "glXChooseFBConfig failed");
    auto config = fbConfigs[0];
    XFree(fbConfigs);
    return config;
}

auto CreateGLContext(
    const std::any& a_Display,
    const std::any& a_Drawable,
    const ContextWrapper* a_SharedContext,
    const bool& a_SetPixelFormat)
{
    InitializeGLX();

    auto display = std::any_cast<Display*>(a_Display);
    auto screen  = DefaultScreen(display);

    // a_Drawable only has a value for CtxNormal (attaching to an existing window);
    // CtxHeadless never sets handleDrawable and never calls this with a real one.
    auto fbConfig = a_Drawable.has_value()
        ? SelectFBConfigForWindow(display, screen, std::any_cast<XID>(a_Drawable))
        : SelectFBConfig(display, screen, a_SetPixelFormat);

    auto sharedCtx = a_SharedContext != nullptr ? std::any_cast<GLXContext>(a_SharedContext->handle) : nullptr;
    auto context   = glXCreateContextAttribsARB(display, fbConfig, sharedCtx, True, glxContextAttribs);
    MSGCheckErrorFatal(context == nullptr, "glXCreateContextAttribsARB failed");
    return context;
}
}

GLX::ContextWrapper::ContextWrapper(
    const std::any& a_XDisplay,
    const std::any& a_XDrawable,
    const ContextWrapper* a_SharedContext, const bool& a_SetPixelFormat)
    : handleDisplay(a_XDisplay)
    , handleDrawable(a_XDrawable)
    , handle(CreateGLContext(a_XDisplay, a_XDrawable, a_SharedContext, a_SetPixelFormat))
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