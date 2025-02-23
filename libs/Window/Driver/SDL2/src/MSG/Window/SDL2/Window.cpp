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

namespace MSG::Window {
static SDL_WindowFlags ConvertFlags(const Flags& a_Flags)
{
    uint32_t flags = 0u;
    if ((a_Flags & FlagsFullscreenBits) != 0)
        flags |= SDL_WINDOW_FULLSCREEN;
    if ((a_Flags & FlagsShownBits) != 0)
        flags |= SDL_WINDOW_SHOWN;
    if ((a_Flags & FlagsHiddenBits) != 0)
        flags |= SDL_WINDOW_HIDDEN;
    if ((a_Flags & FlagsBorderlessBits) != 0)
        flags |= SDL_WINDOW_BORDERLESS;
    if ((a_Flags & FlagsResizableBits) != 0)
        flags |= SDL_WINDOW_RESIZABLE;
    if ((a_Flags & FlagsMinimizedBits) != 0)
        flags |= SDL_WINDOW_MINIMIZED;
    if ((a_Flags & FlagsMaximizedBits) != 0)
        flags |= SDL_WINDOW_MAXIMIZED;
    if ((a_Flags & FlagsMouseGrabbedBits) != 0)
        flags |= SDL_WINDOW_MOUSE_GRABBED;
    if ((a_Flags & FlagsInputFocusBits) != 0)
        flags |= SDL_WINDOW_INPUT_FOCUS;
    if ((a_Flags & FlagsMouseFocusBits) != 0)
        flags |= SDL_WINDOW_MOUSE_FOCUS;
    if ((a_Flags & FlagsFullscreenDesktopBits) != 0)
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    if ((a_Flags & FlagsAllowHighdpiBits) != 0)
        flags |= SDL_WINDOW_ALLOW_HIGHDPI;
    if ((a_Flags & FlagsMouseCaptureBits) != 0)
        flags |= SDL_WINDOW_MOUSE_CAPTURE;
    if ((a_Flags & FlagsAlwaysOnTopBits) != 0)
        flags |= SDL_WINDOW_ALWAYS_ON_TOP;
    if ((a_Flags & FlagsSkipTaskbarBits) != 0)
        flags |= SDL_WINDOW_SKIP_TASKBAR;
    if ((a_Flags & FlagsUtilityBits) != 0)
        flags |= SDL_WINDOW_UTILITY;
    if ((a_Flags & FlagsTooltipBits) != 0)
        flags |= SDL_WINDOW_TOOLTIP;
    if ((a_Flags & FlagsPopupMenuBits) != 0)
        flags |= SDL_WINDOW_POPUP_MENU;
    if ((a_Flags & FlagsKeyboardGrabbedBits) != 0)
        flags |= SDL_WINDOW_KEYBOARD_GRABBED;
    return SDL_WindowFlags(flags);
}

static SDL_Window* CreateSDLWindow(const CreateWindowInfo& a_Info)
{
    SDL_InitSubSystem(SDL_INIT_VIDEO);
    return SDL_CreateWindow(
        a_Info.name.c_str(),
        a_Info.positionX == -1 ? SDL_WINDOWPOS_CENTERED : a_Info.positionX,
        a_Info.positionY == -1 ? SDL_WINDOWPOS_CENTERED : a_Info.positionY,
        a_Info.width, a_Info.height,
        ConvertFlags(a_Info.flags)); // no API specific flags because we want to set the pixel format ourselves
}

class EventListener : public Events::EventListener {
public:
    EventListener()
        : Events::EventListener(SDL_WINDOWEVENT)
    {
    }
    virtual ~EventListener() = default;
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
    : _sdlWindow(CreateSDLWindow(a_Info))
    , _eventListener(GetEventListener())
    , _renderer(a_Renderer)
    , _vSync(a_Info.vSync)
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

std::any Impl::GetNativeDisplayHandle() const
{
    SDL_SysWMinfo wmInfo;
    GetWMInfo(wmInfo);
#ifdef _WIN32
    return wmInfo.info.win.hdc;
#elif defined(__linux__)
    return wmInfo.info.x11.display;
#endif
}

std::any Impl::GetNativeWindowHandle() const
{
    SDL_SysWMinfo wmInfo;
    GetWMInfo(wmInfo);
#ifdef _WIN32
    return wmInfo.info.win.window;
#elif defined(__linux__)
    return wmInfo.info.x11.window;
#endif
}

void Impl::_ResizeCallback(const uint32_t& a_Width, const uint32_t& a_Height)
{
    if (a_Width == 0 || a_Height == 0 || IsClosing())
        return;
    _width  = a_Width;
    _height = a_Height;
    SwapChain::CreateSwapChainInfo swapChainInfo;
    swapChainInfo.presentMode                    = _vSync ? SwapChain::PresentMode::MailBox : SwapChain::PresentMode::Immediate;
    swapChainInfo.width                          = _width;
    swapChainInfo.height                         = _height;
    swapChainInfo.imageCount                     = 3;
    swapChainInfo.windowInfo.setPixelFormat      = true;
    swapChainInfo.windowInfo.nativeDisplayHandle = GetNativeDisplayHandle();
    swapChainInfo.windowInfo.nativeWindowHandle  = GetNativeWindowHandle();
    if (_swapChain == nullptr)
        _swapChain = SwapChain::Create(_renderer, swapChainInfo);
    else
        _swapChain = SwapChain::Recreate(_swapChain, swapChainInfo);
}

void Impl::WaitSwapChain() const
{
    SwapChain::Wait(_swapChain);
}

void Impl::Show() const
{
    SDL_ShowWindow(_sdlWindow);
}

void Impl::Present(const Renderer::RenderBuffer::Handle& a_RenderBuffer)
{
    WaitSwapChain();
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
        _swapChain.reset();
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
    return std::make_shared<MSG::Window::Impl>(a_Renderer, a_Info);
}

void MSG::Window::WaitSwapChain(const Handle& a_Window)
{
    return a_Window->WaitSwapChain();
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
    return a_Window->GetNativeWindowHandle();
}

std::any MSG::Window::GetNativeDisplayHandle(const Handle& a_Window)
{
    return a_Window->GetNativeDisplayHandle();
}
