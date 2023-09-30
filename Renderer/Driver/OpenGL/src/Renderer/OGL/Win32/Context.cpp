#include <Renderer/OGL/RAII/DebugGroup.hpp>
#include <Renderer/OGL/Win32/Context.hpp>
#include <Renderer/OGL/Win32/Error.hpp>
#include <Renderer/OGL/Win32/Window.hpp>
#include <Renderer/Structs.hpp>

#include <GL/glew.h>
#include <GL/wglew.h>
#include <windows.h>

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace TabGraph::Renderer::RAII {
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
void InitializeOGL()
{
    static bool s_Initialized = false;
    if (s_Initialized)
        return; // we only need to initialize OGL once for the whole execution
    auto tempWindow     = Window("OpenGL::Initialize", "OpenGL::Initialize");
    const auto tempHWND = HWND(tempWindow.hwnd);
    const auto tempDC   = GetDC(tempHWND);
    {
        PIXELFORMATDESCRIPTOR pfd;
        std::memset(&pfd, 0, sizeof(pfd));
        pfd.nSize          = sizeof(pfd);
        pfd.nVersion       = 1;
        pfd.dwFlags        = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType     = PFD_TYPE_RGBA;
        pfd.iLayerType     = PFD_MAIN_PLANE;
        pfd.cColorBits     = 32;
        const auto pformat = ChoosePixelFormat(tempDC, &pfd);
        WIN32_CHECK_ERROR(pformat != 0);
        WIN32_CHECK_ERROR(SetPixelFormat(tempDC, pformat, nullptr));
    }
    const auto tempHGLRC = wglCreateContext(tempDC);
    glewExperimental     = true;
    WIN32_CHECK_ERROR(tempHGLRC != nullptr);
    WIN32_CHECK_ERROR(wglMakeCurrent(tempDC, tempHGLRC));
    WIN32_CHECK_ERROR(glewInit() == GLEW_OK); // load OGL extensions
    WIN32_CHECK_ERROR(wglewInit() == GLEW_OK); // load WGL extensions
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(tempHGLRC);
    ReleaseDC(tempHWND, tempDC);
    s_Initialized = true; // OGL was initialized, no need to do it again next time
}

void* CreateContext(const void* a_HDC)
{
    if (!WGLEW_ARB_create_context)
        throw std::runtime_error("Modern context creation not supported !");
    if (!WGLEW_ARB_create_context_robustness)
        throw std::runtime_error("Robust context creation not supported !");
    const int attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
#ifdef _DEBUG
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB | WGL_CONTEXT_DEBUG_BIT_ARB,
#else
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB,
#endif _DEBUG
        0
    };
    auto hglrc = wglCreateContextAttribsARB(HDC(a_HDC), nullptr, attribs);
    WIN32_CHECK_ERROR(hglrc != nullptr);
    return hglrc;
}

void SetOffscreenDefaultPixelFormat(const void* a_HDC)
{
    const auto hdc              = HDC(a_HDC);
    constexpr int attribIList[] = {
        WGL_SUPPORT_OPENGL_ARB, true,
        WGL_COLOR_BITS_ARB, 0,
        WGL_ALPHA_BITS_ARB, 0,
        WGL_DEPTH_BITS_ARB, 0,
        WGL_STENCIL_BITS_ARB, 0,
        WGL_ACCUM_BITS_ARB, 0,
        WGL_AUX_BUFFERS_ARB, 0,
        0
    };
    int32_t pixelFormat     = 0;
    uint32_t pixelFormatNbr = 0;
    WIN32_CHECK_ERROR(wglChoosePixelFormatARB(hdc, attribIList, nullptr, 1, &pixelFormat, &pixelFormatNbr));
    WIN32_CHECK_ERROR(pixelFormat != 0);
    WIN32_CHECK_ERROR(pixelFormatNbr != 0);
    WIN32_CHECK_ERROR(SetPixelFormat(hdc, pixelFormat, nullptr));
}

void SetDefaultPixelFormat(const void* a_HDC, const PixelFormat& a_PixelFormat)
{
    const auto hdc          = HDC(a_HDC);
    const int attribIList[] = {
        WGL_DRAW_TO_WINDOW_ARB, true,
        WGL_SUPPORT_OPENGL_ARB, true,
        WGL_DOUBLE_BUFFER_ARB, true,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_COLORSPACE_EXT, (a_PixelFormat.sRGB ? WGL_COLORSPACE_SRGB_EXT : WGL_COLORSPACE_LINEAR_EXT),
        WGL_COLOR_BITS_ARB, a_PixelFormat.colorBits,
        WGL_ALPHA_BITS_ARB, a_PixelFormat.alphaBits,
        WGL_DEPTH_BITS_ARB, a_PixelFormat.depthBits,
        WGL_STENCIL_BITS_ARB, a_PixelFormat.stencilBits,
        0
    };
    int32_t pixelFormat     = 0;
    uint32_t pixelFormatNbr = 0;
    WIN32_CHECK_ERROR(wglChoosePixelFormatARB(hdc, attribIList, nullptr, 1, &pixelFormat, &pixelFormatNbr));
    WIN32_CHECK_ERROR(pixelFormat != 0);
    WIN32_CHECK_ERROR(pixelFormatNbr != 0);
    WIN32_CHECK_ERROR(SetPixelFormat(hdc, pixelFormat, nullptr));
}

Context::Context(
    const void* a_HWND,
    const bool& a_SetPixelFormat,
    const PixelFormat& a_PixelFormat,
    const bool& a_Offscreen,
    const uint32_t& a_MaxPendingTasks)
    : maxPendingTasks(a_MaxPendingTasks)
    , hwnd(HWND(a_HWND))
    , hdc(GetDC(HWND(hwnd)))
{
    WIN32_CHECK_ERROR(hdc != nullptr);
    InitializeOGL();
    if (a_SetPixelFormat) {
        if (a_Offscreen)
            SetOffscreenDefaultPixelFormat(hdc);
        else
            SetDefaultPixelFormat(hdc, a_PixelFormat);
    }
    hglrc = CreateContext(hdc);
    workerThread.PushCommand(
        [this] {
            wglMakeCurrent(HDC(hdc), HGLRC(hglrc));
#ifdef _DEBUG
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(MessageCallback, 0);
#endif _DEBUG
        });
}

Context::Context(Context&& a_Other)
{
    a_Other.Release();
    pendingCmds     = std::move(a_Other.pendingCmds);
    maxPendingTasks = std::move(a_Other.maxPendingTasks);
    hwnd            = std::move(a_Other.hwnd);
    hdc             = std::move(a_Other.hdc);
    hglrc           = std::move(a_Other.hglrc);
    workerThread.PushCommand(
        [this] {
            wglMakeCurrent(HDC(hdc), HGLRC(hglrc));
        });
}

Context::~Context()
{
    ExecuteCmds();
    if (hglrc != nullptr) {
        Release();
        workerThread.Wait();
        WIN32_CHECK_ERROR(wglDeleteContext(HGLRC(hglrc)));
        ReleaseDC(HWND(hwnd), HDC(hdc));
    }
}

void Context::Release()
{
    workerThread.PushSynchronousCommand(
        [this] {
            wglMakeCurrent(nullptr, nullptr);
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
        return;
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
            auto debugGroup = RAII::DebugGroup("Wait for context : " + std::to_string((unsigned long long)hglrc));
            auto sync       = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
            glClientWaitSync(sync, GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);
            glDeleteSync(sync);
        });
}
}
