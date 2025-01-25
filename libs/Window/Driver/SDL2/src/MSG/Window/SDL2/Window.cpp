#include <MSG/Events/EventManager.hpp>
#include <MSG/Events/SDL2/EventManager.hpp>
#include <MSG/SwapChain/SwapChain.hpp>
#include <MSG/Window/Events.hpp>
#include <MSG/Window/SDL2/Window.hpp>
#include <MSG/Window/Structs.hpp>
#include <MSG/Window/Window.hpp>

#include <algorithm>
#include <iostream>
#include <unordered_set>

#define NOMSG
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#ifdef _WIN32
#include <MSG/OGLContext/Win32/PlatformCtx.hpp>
#include <MSG/OGLContext/Win32/Win32.hpp>

#define WIN32_STYLE_FULLSCREEN (WS_POPUP | WS_MINIMIZEBOX)
#define WIN32_STYLE_BORDERLESS (WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)
#define WIN32_STYLE_NORMAL     (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)
#define WIN32_STYLE_RESIZABLE  (WS_THICKFRAME | WS_MAXIMIZEBOX)

namespace MSG::Window {
static uint32_t GetNativeStyle(const Flags& a_Flags)
{
    uint32_t style  = 0;
    bool fullscreen = (a_Flags & FlagsFullscreenBits) != 0;
    bool borderless = (a_Flags & FlagsBorderlessBits) != 0;
    bool resizable  = (a_Flags & FlagsResizableBits) != 0;
    bool shown      = (a_Flags & FlagsShownBits) != 0;
    bool hidden     = (a_Flags & FlagsHiddenBits) != 0;
    bool minimized  = (a_Flags & FlagsMinimizedBits) != 0;
    if (fullscreen) {
        style |= WIN32_STYLE_FULLSCREEN;
        return style;
    }
    if (borderless)
        style |= WIN32_STYLE_BORDERLESS;
    else
        style |= WIN32_STYLE_NORMAL;
    if (resizable) {
        assert(!borderless);
        style |= WIN32_STYLE_RESIZABLE;
    }
    if (minimized)
        style |= WS_MINIMIZE;
    return style;
}

static SDL_Window* CreateSDLWindow(const Renderer::Handle& a_Renderer, const CreateWindowInfo& a_Info)
{
    SDL_InitSubSystem(SDL_INIT_VIDEO);
    Win32::CreateHWNDInfo wndInfo {
        .className = Win32::DefaultWindowClassName,
        .name      = a_Info.name,
        .style     = GetNativeStyle(a_Info.flags),
        .width     = a_Info.width,
        .height    = a_Info.height
    };
    auto hwnd       = Win32::CreateHWND(wndInfo);
    auto sdlWindow  = SDL_CreateWindowFrom(std::any_cast<HWND>(hwnd));
    bool fullscreen = (a_Info.flags & FlagsFullscreenBits) != 0;
    bool borderless = (a_Info.flags & FlagsBorderlessBits) != 0;
    bool resizable  = (a_Info.flags & FlagsResizableBits) != 0;
    bool minimized  = (a_Info.flags & FlagsMinimizedBits) != 0;
    bool maximized  = (a_Info.flags & FlagsMaximizedBits) != 0;
    bool shown      = (a_Info.flags & FlagsShownBits) != 0;
    bool hidden     = (a_Info.flags & FlagsHiddenBits) != 0;
    bool grabMouse  = (a_Info.flags & FlagsMouseGrabbedBits) != 0;
    bool grabKbd    = (a_Info.flags & FlagsKeyboardGrabbedBits) != 0;
    auto positionX  = a_Info.positionX == -1 ? SDL_WINDOWPOS_CENTERED : a_Info.positionX;
    auto positionY  = a_Info.positionY == -1 ? SDL_WINDOWPOS_CENTERED : a_Info.positionY;
    if (fullscreen) {
        assert(!maximized);
        assert(!minimized);
        SDL_SetWindowFullscreen(sdlWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
    if (minimized) {
        assert(!maximized);
        SDL_MinimizeWindow(sdlWindow);
    }
    if (maximized) {
        assert(!minimized);
        SDL_MaximizeWindow(sdlWindow);
    }
    if (hidden) {
        assert(!shown);
        SDL_HideWindow(sdlWindow);
    }
    if (shown) {
        assert(!hidden);
        SDL_ShowWindow(sdlWindow);
    }
    SDL_SetWindowBordered(sdlWindow, SDL_bool(!borderless));
    SDL_SetWindowMouseGrab(sdlWindow, SDL_bool(grabMouse));
    SDL_SetWindowSize(sdlWindow, a_Info.width, a_Info.height);
    SDL_SetWindowPosition(sdlWindow, positionX, positionY);
    return sdlWindow;
}
}
#endif

namespace MSG::Window {
class EventListener : Events::EventListener {
public:
    EventListener()
        : Events::EventListener(SDL_WINDOWEVENT)
    {
    }
    void AddWindow(Impl* a_Window) { _windowList.insert(a_Window); }
    void RemoveWindow(Impl* a_Window) { _windowList.erase(a_Window); }
    void operator()(const SDL_Event& a_Event, std::any a_UserData) override
    {
        auto& windowEvent = a_Event.window;
        auto windowItr    = std::find_if(_windowList.begin(), _windowList.end(), [windowEvent](auto& a_Window) { return a_Window->GetID() == windowEvent.windowID; });
        if (windowItr == _windowList.end())
            return;
        (*windowItr)->HandleEvent(a_Event.window);
    }

private:
    std::unordered_set<Impl*> _windowList;
};

static std::shared_ptr<EventListener> GetEventListener()
{
    static std::weak_ptr<EventListener> s_EventListener;
    if (s_EventListener.expired()) {
        auto ptr        = std::make_shared<EventListener>();
        s_EventListener = ptr;
        return ptr;
    }
    return s_EventListener.lock();
}

Impl::Impl(const Renderer::Handle& a_Renderer, const CreateWindowInfo& a_Info)
    : _sdlWindow(CreateSDLWindow(a_Renderer, a_Info))
    , _eventListener(GetEventListener())
    , _renderer(a_Renderer)
{
    _eventListener->AddWindow(this);
    SDL_GetWindowSizeInPixels(_sdlWindow, &_width, &_height);
    _ResizeCallback(_width, _height);
}

Impl::~Impl()
{
    _eventListener->RemoveWindow(this);
    SDL_DestroyWindow(_sdlWindow);
}

void Impl::GetWMInfo(SDL_SysWMinfo& a_Info) const
{
    a_Info.version.major = SDL_MAJOR_VERSION;
    a_Info.version.minor = SDL_MINOR_VERSION;
    a_Info.version.patch = SDL_PATCHLEVEL;
    SDL_GetWindowWMInfo(_sdlWindow, &a_Info);
}

void Impl::_ResizeCallback(const uint32_t& a_Width, const uint32_t& a_Height)
{
    if (a_Width == 0 || a_Height == 0 || IsClosing())
        return;
    _width  = a_Width;
    _height = a_Height;
    SDL_SysWMinfo wmInfo;
    GetWMInfo(wmInfo);
    SwapChain::CreateSwapChainInfo swapChainInfo;
    swapChainInfo.vSync                     = _vSync;
    swapChainInfo.width                     = _width;
    swapChainInfo.height                    = _height;
    swapChainInfo.imageCount                = 3;
    swapChainInfo.windowInfo.setPixelFormat = true;
#ifdef _WIN32
    swapChainInfo.windowInfo.nativeDisplayHandle = wmInfo.info.win.hdc;
    swapChainInfo.windowInfo.nativeWindowHandle  = wmInfo.info.win.window;
#elif defined(__linux__)
    swapChainInfo.windowInfo.nativeDisplayHandle = wmInfo.info.x11.display;
    swapChainInfo.windowInfo.nativeWindowHandle  = wmInfo.info.x11.window;
#endif
    if (_swapChain == nullptr)
        _swapChain = SwapChain::Create(_renderer, swapChainInfo);
    else
        _swapChain = SwapChain::Recreate(_swapChain, swapChainInfo);
}

void Impl::Show() const
{
    SDL_ShowWindow(_sdlWindow);
}

void Impl::Present(const Renderer::RenderBuffer::Handle& a_RenderBuffer)
{
    SwapChain::Wait(_swapChain);
    SwapChain::Present(_swapChain, a_RenderBuffer);
}

bool Impl::IsClosing() const
{
    return _isClosing;
}

unsigned Impl::GetID() const
{
    return SDL_GetWindowID(_sdlWindow);
}

int32_t Impl::GetWidth() const
{
    return _width;
}

int32_t Impl::GetHeight() const
{
    return _height;
}

SwapChain::Handle Impl::GetSwapChain() const
{
    return _swapChain;
}

void Impl::HandleEvent(const SDL_WindowEvent& a_Event)
{
    Event* eventPtr = nullptr;
    switch (a_Event.event) {
    case SDL_WINDOWEVENT_NONE: {
    } break;
    case SDL_WINDOWEVENT_SHOWN: {
        eventPtr = (Event*)new EventWindowShown(shared_from_this());
    } break;
    case SDL_WINDOWEVENT_HIDDEN: {
        eventPtr = (Event*)new EventWindowHidden(shared_from_this());
    } break;
    case SDL_WINDOWEVENT_EXPOSED: {
        eventPtr = (Event*)new EventWindowExposed(shared_from_this());
    } break;
    case SDL_WINDOWEVENT_MOVED: {
        eventPtr = (Event*)new EventWindowMoved(shared_from_this(), a_Event.data1, a_Event.data2);
    } break;
    case SDL_WINDOWEVENT_RESIZED: {
        eventPtr = (Event*)new EventWindowResized(shared_from_this(), a_Event.data1, a_Event.data2);
        _ResizeCallback(a_Event.data1, a_Event.data2);
    } break;
    case SDL_WINDOWEVENT_SIZE_CHANGED: {
        eventPtr = (Event*)new EventWindowSizeChanged(shared_from_this(), a_Event.data1, a_Event.data2);
        _ResizeCallback(a_Event.data1, a_Event.data2);
    } break;
    case SDL_WINDOWEVENT_MINIMIZED: {
        eventPtr = (Event*)new EventWindowMinimized(shared_from_this());
    } break;
    case SDL_WINDOWEVENT_MAXIMIZED: {
        eventPtr = (Event*)new EventWindowMaximized(shared_from_this());
    } break;
    case SDL_WINDOWEVENT_RESTORED: {
        eventPtr = (Event*)new EventWindowRestored(shared_from_this());
    } break;
    case SDL_WINDOWEVENT_ENTER: {
        eventPtr = (Event*)new EventWindowEntered(shared_from_this());
    } break;
    case SDL_WINDOWEVENT_LEAVE: {
        eventPtr = (Event*)new EventWindowLeft(shared_from_this());
    } break;
    case SDL_WINDOWEVENT_FOCUS_GAINED: {
        eventPtr = (Event*)new EventWindowFocusGained(shared_from_this());
    } break;
    case SDL_WINDOWEVENT_FOCUS_LOST: {
        eventPtr = (Event*)new EventWindowFocusLost(shared_from_this());
    } break;
    case SDL_WINDOWEVENT_CLOSE: {
        eventPtr   = (Event*)new EventWindowClosed(shared_from_this());
        _isClosing = true;
    } break;
    case SDL_WINDOWEVENT_TAKE_FOCUS: {
        eventPtr = (Event*)new EventWindowTookFocus(shared_from_this());
    } break;
    case SDL_WINDOWEVENT_HIT_TEST:
    case SDL_WINDOWEVENT_ICCPROF_CHANGED:
    case SDL_WINDOWEVENT_DISPLAY_CHANGED:
    default:
        std::cout << "Unknown window event\n";
        break;
    }
    if (eventPtr != nullptr) {
        auto uniquePtr = std::unique_ptr<Event>(eventPtr);
        Events::PushNoLock(uniquePtr);
    }
}
}

MSG::Window::Handle MSG::Window::Create(const Renderer::Handle& a_Renderer, const CreateWindowInfo& a_Info)
{
    return Handle(new MSG::Window::Impl(a_Renderer, a_Info));
}

void MSG::Window::Show(const Handle& a_Window)
{
    return a_Window->Show();
}

void MSG::Window::Present(const Handle& a_Window, const Renderer::RenderBuffer::Handle& a_RenderBuffer)
{
    return a_Window->Present(a_RenderBuffer);
}

bool MSG::Window::IsClosing(const Handle& a_Window)
{
    return a_Window->IsClosing();
}

uint32_t MSG::Window::GetWidth(const Handle& a_Window)
{
    return a_Window->GetWidth();
}

uint32_t MSG::Window::GetHeight(const Handle& a_Window)
{
    return a_Window->GetHeight();
}

MSG::SwapChain::Handle MSG::Window::GetSwapChain(const Handle& a_Window)
{
    return a_Window->GetSwapChain();
}

std::any MSG::Window::GetNativeWindowHandle(const Handle& a_Window)
{
    SDL_SysWMinfo info;
    a_Window->GetWMInfo(info);
#ifdef _WIN32
    return HWND(info.info.win.window);
#elif defined __linux
    return info.info.x11.window;
#endif
}

std::any MSG::Window::GetNativeDisplayHandle(const Handle& a_Window)
{
    SDL_SysWMinfo info;
    a_Window->GetWMInfo(info);
#ifdef _WIN32
    return HDC(info.info.win.hdc);
#elif defined __linux
    return info.info.x11.display;
#endif
}
