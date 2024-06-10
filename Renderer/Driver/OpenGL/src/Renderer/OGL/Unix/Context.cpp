#include <Renderer/OGL/RAII/DebugGroup.hpp>
#include <Renderer/OGL/Unix/Context.hpp>
#include <Renderer/Structs.hpp>

#include <Tools/Debug.hpp>

#include <GL/eglew.h>
#include <GL/glew.h>
#include <GL/glxew.h>

#include <cassert>
#include <iostream>
#include <sstream>
#include <stdexcept>

#define APIVersion(major, minor) (major * 100 + minor * 10)
constexpr auto GLMajor = 4;
constexpr auto GLMinor = 3;

constexpr auto GLXMajor = 1;
constexpr auto GLXMinor = 4;

namespace TabGraph::Renderer {
int XErrorHandler(Display* dsp, XErrorEvent* error)
{
    char errorstring[128];
    XGetErrorText(dsp, error->error_code, errorstring, 128);
    std::cerr << "ack!fatal: X error--" << errorstring << std::endl;
    throw std::runtime_error(errorstring);
    return 0;
}

void GLAPIENTRY MessageCallback(
    GLenum source,
    GLenum type,
    GLenum id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    if (type == GL_DEBUG_TYPE_ERROR) {
        std::stringstream ss {};
        ss << "GL CALLBACK : **GL ERROR * *\n"
           << " type     = " << type << "\n"
           << " severity = " << severity << "\n"
           << " message  = " << message;
        std::cerr << ss.str() << std::endl;
        throw std::runtime_error(ss.str());
    }
}

void InitializeGL()
{
    static bool s_Initialized = false;
    if (s_Initialized)
        return;
    glewExperimental = true;
    auto err         = glewInit();
#ifndef NDEBUG
    std::cerr << "glewInit : " << glewGetErrorString(err) << std::endl;
#endif
    s_Initialized = true;
}

bool InitializeGLX()
{
    static bool initialized = false;
    if (initialized)
        return true;
    auto display = XOpenDisplay(nullptr);
    assert(display != nullptr);
    XSetErrorHandler(XErrorHandler);

    //----- Create and make current a temporary GL context to initialize GLXEW
    // visual
    std::vector<int> attributes = {
        GLX_RGBA, None
    };

    const int screen        = DefaultScreen(display);
    XVisualInfo* visualInfo = glXChooseVisual(display, screen, attributes.data());
    if (!visualInfo) {
        std::cerr << "glXChooseVisual failed" << std::endl;
        return false;
    }

    // context
    GLXContext context = glXCreateContext(display, visualInfo, nullptr, True);
    if (!context) {
        std::cerr << "glXCreateContext failed" << std::endl;
        return false;
    }

    // window
    const XID parent = RootWindow(display, screen);
    XSetWindowAttributes wa;
    wa.colormap          = XCreateColormap(display, parent, visualInfo->visual, AllocNone);
    wa.background_pixmap = None;
    wa.border_pixel      = 0;
    XID drawable         = XCreateWindow(display, parent, 0, 0, 16, 16,
                0, visualInfo->depth, InputOutput,
                visualInfo->visual,
                CWBackPixmap | CWBorderPixel | CWColormap,
                &wa);
    if (!drawable) {
        std::cerr << "XCreateWindow failed" << std::endl;
        return false;
    }

    XFree(visualInfo);
    XSync(display, False);

    glXMakeCurrent(display, drawable, context);

    const GLenum result     = glxewInit();
    bool success            = result == GLEW_OK;
    std::string errorString = reinterpret_cast<const char*>(glewGetErrorString(result));
    debugLog("glxewInit : " + errorString);

    InitializeGL();

    XSync(display, False);
    glXDestroyContext(display, context);
    XDestroyWindow(display, drawable);

    return initialized = success;
}

const static std::vector<int> context_attribs = {
    GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
    GLX_CONTEXT_MINOR_VERSION_ARB, 3,
#ifndef NDEBUG
    GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_DEBUG_BIT_ARB,
#endif // NDEBUG
    GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
    None
};

auto CreateOGLContext(
    Display* a_Display,
    GLXContext a_SharedContext,
    const bool& a_SetPixelFormat,
    const PixelFormat& a_PixelFormat)
{
    assert(InitializeGLX());
    auto screen            = DefaultScreen(a_Display);
    GLXFBConfig* fbConfigs = nullptr;
    if (a_SetPixelFormat) {
        const static std::vector<int> configAttribs = {
            GLX_X_RENDERABLE, True,
            GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
            GLX_RENDER_TYPE, GLX_RGBA_BIT,
            GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
            GLX_RED_SIZE, a_PixelFormat.redBits,
            GLX_GREEN_SIZE, a_PixelFormat.greenBits,
            GLX_BLUE_SIZE, a_PixelFormat.blueBits,
            GLX_ALPHA_SIZE, a_PixelFormat.alphaBits,
            GLX_DEPTH_SIZE, a_PixelFormat.depthBits,
            GLX_STENCIL_SIZE, a_PixelFormat.stencilBits,
            GLX_DOUBLEBUFFER, True,
            None
        };
        int configNbr = 0;
        fbConfigs     = glXChooseFBConfig(a_Display, screen, configAttribs.data(), &configNbr);
    } else {
        const static std::vector<int> configAttribs = { None };
        int configNbr                               = 0;
        fbConfigs                                   = glXChooseFBConfig(a_Display, screen, configAttribs.data(), &configNbr);
    }
    assert(fbConfigs != nullptr && "Could not find proper display configuration");
    return glXCreateContextAttribsARB(a_Display, fbConfigs[0], a_SharedContext, True, context_attribs.data());
}

auto CreateOGLContext(Display* a_Display, GLXContext a_SharedContext)
{
    assert(a_Display != nullptr);
    assert(InitializeGLX());
    const static std::vector<int> configAttribs = { None };
    auto screen                                 = DefaultScreen(a_Display);
    int configNbr                               = 0;
    auto fbConfigs                              = glXChooseFBConfig(a_Display, screen, configAttribs.data(), &configNbr);
    assert(fbConfigs != nullptr && "Could not find proper display configuration");
    return glXCreateContextAttribsARB(a_Display, fbConfigs[0], a_SharedContext, True, context_attribs.data());
}

Context::Context(
    void* a_X11Display,
    void* a_SharedContext,
    uint64_t a_WindowID,
    const bool& a_SetPixelFormat,
    const PixelFormat& a_PixelFormat,
    const uint32_t& a_MaxPendingTasks)
    : maxPendingTasks(a_MaxPendingTasks)
    , drawableID(a_WindowID)
    , display(a_X11Display)
    , context(CreateOGLContext((Display*)display, (GLXContext)a_SharedContext, a_SetPixelFormat, a_PixelFormat))
{
    workerThread.PushCommand(
        [this] {
            glXMakeCurrent((Display*)display, drawableID, GLXContext(context));
#ifndef NDEBUG
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(MessageCallback, 0);
#endif // NDEBUG
        });
}

Context::Context(void* a_X11Display, void* a_SharedContext, const uint32_t& a_MaxPendingTasks)
    : maxPendingTasks(a_MaxPendingTasks)
    , display(a_X11Display)
    , context(CreateOGLContext((Display*)a_X11Display, (GLXContext)a_SharedContext))
{
    workerThread.PushCommand(
        [this] {
            glXMakeCurrent((Display*)display, drawableID, GLXContext(context));
#ifndef NDEBUG
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(MessageCallback, 0);
#endif // NDEBUG
        });
}

Context::Context(Context&& a_Other)
    : maxPendingTasks(a_Other.maxPendingTasks)
    , drawableID(a_Other.drawableID)
    , display(std::move(a_Other.display))
    , pendingCmds(std::move(a_Other.pendingCmds))
{
    a_Other.Release();
    context = std::move(a_Other.context);
    workerThread.PushCommand(
        [this] {
            glXMakeCurrent((Display*)display, drawableID, GLXContext(context));
        });
}

Context::~Context()
{
    ExecuteCmds();
    if (context != nullptr) {
        Release();
        workerThread.Wait();
    }
}

void Context::Release()
{
    workerThread.PushSynchronousCommand(
        [this] {
            glXMakeCurrent((Display*)display, None, nullptr);
        });
}

void Context::PushCmd(const std::function<void()>& a_Command)
{
    pendingCmds.push_back(a_Command);
}

void Context::PushImmediateCmd(const std::function<void()>& a_Command, const bool& a_Synchronous)
{
    if (a_Synchronous)
        workerThread.PushSynchronousCommand(a_Command);
    else
        workerThread.PushCommand(a_Command);
}

void Context::ExecuteCmds(bool a_Synchronous)
{
    if (pendingCmds.empty())
        return a_Synchronous ? WaitWorkerThread() : void();
    auto command = [commands = std::move(pendingCmds)] {
        for (auto& task : commands)
            task();
    };
    a_Synchronous ? workerThread.PushSynchronousCommand(command) : workerThread.PushCommand(command);
}

bool Context::Busy()
{
    return workerThread.PendingTaskCount() > maxPendingTasks;
}

void Context::WaitWorkerThread()
{
    workerThread.Wait();
}

void Context::Wait()
{
    workerThread.PushSynchronousCommand(
        [this] {
            auto debugGroup = RAII::DebugGroup("Wait for context : " + std::to_string((unsigned long long)context));
            auto sync       = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
            glClientWaitSync(sync, GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);
            glDeleteSync(sync);
        });
}
}
