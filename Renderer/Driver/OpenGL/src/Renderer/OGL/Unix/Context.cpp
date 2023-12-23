#include <Renderer/OGL/RAII/DebugGroup.hpp>
#include <Renderer/OGL/Unix/Context.hpp>
#include <Renderer/Structs.hpp>

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
    exit(-1);
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
#ifdef _DEBUG
    std::cerr << "glewInit : " << glewGetErrorString(err) << std::endl;
#endif
    s_Initialized = true;
}

bool InitializeGLX(Display* a_Display)
{
    assert(a_Display != nullptr);
    XSetErrorHandler(XErrorHandler);

    //----- Create and make current a temporary GL context to initialize GLXEW
    // visual
    std::vector<int> attributes = {
        GLX_RGBA, None
    };

    const int screen        = DefaultScreen(a_Display);
    XVisualInfo* visualInfo = glXChooseVisual(a_Display, screen, attributes.data());
    if (!visualInfo) {
        std::cerr << "glXChooseVisual failed" << std::endl;
        return false;
    }

    // context
    GLXContext context = glXCreateContext(a_Display, visualInfo, nullptr, True);
    if (!context) {
        std::cerr << "glXCreateContext failed" << std::endl;
        return false;
    }

    // window
    const XID parent = RootWindow(a_Display, screen);
    XSetWindowAttributes wa;
    wa.colormap          = XCreateColormap(a_Display, parent, visualInfo->visual, AllocNone);
    wa.background_pixmap = None;
    wa.border_pixel      = 0;
    XID drawable         = XCreateWindow(a_Display, parent, 0, 0, 16, 16,
                0, visualInfo->depth, InputOutput,
                visualInfo->visual,
                CWBackPixmap | CWBorderPixel | CWColormap,
                &wa);
    if (!drawable) {
        std::cerr << "XCreateWindow failed" << std::endl;
        return false;
    }

    XFree(visualInfo);
    XSync(a_Display, False);

    glXMakeCurrent(a_Display, drawable, context);

    const GLenum result = glxewInit();
    bool success        = result == GLEW_OK;
    std::cerr << "glxewInit : " << glewGetErrorString(result) << std::endl;
    InitializeGL();

    XSync(a_Display, False);
    glXDestroyContext(a_Display, context);
    XDestroyWindow(a_Display, drawable);

    return success;
}

auto CreateOGLContext(
    Display* a_Display,
    const bool& a_SetPixelFormat,
    const PixelFormat& a_PixelFormat)
{
    InitializeGLX(a_Display);
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
    const static std::vector<int> context_attribs = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
        GLX_CONTEXT_MINOR_VERSION_ARB, 3,
#ifdef _DEBUG
        GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_DEBUG_BIT_ARB,
#endif //_DEBUG
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        None
    };
    return glXCreateContextAttribsARB(a_Display, fbConfigs[0], nullptr, True, context_attribs.data());
}

auto CreateOGLContext(Display* a_Display)
{
    return CreateOGLContext(a_Display, false, {});
}

Context::Context(
    void* a_X11Display,
    uint64_t a_WindowID,
    const bool& a_SetPixelFormat,
    const PixelFormat& a_PixelFormat,
    const uint32_t& a_MaxPendingTasks)
    : maxPendingTasks(a_MaxPendingTasks)
    , drawableID(a_WindowID)
    , display(a_X11Display)
    , context(CreateOGLContext((Display*)display, a_SetPixelFormat, a_PixelFormat))
{
    workerThread.PushCommand(
        [this] {
            glXMakeCurrent((Display*)display, drawableID, GLXContext(context));
#ifdef _DEBUG
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(MessageCallback, 0);
#endif //_DEBUG
        });
}

Context::Context(const uint32_t& a_MaxPendingTasks)
    : Context(XOpenDisplay(nullptr), 0, false, {}, a_MaxPendingTasks)
{
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
