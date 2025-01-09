#include <Events/EventManager.hpp>
#include <Events/SDL2/EventManager.hpp>
#include <Renderer/SwapChain.hpp>
#include <Window/Events.hpp>
#include <Window/SDL2/Window.hpp>
#include <Window/Structs.hpp>
#include <Window/Window.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <iostream>
#include <unordered_set>

namespace TabGraph::Window {
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

SDL_WindowFlags ConvertFlags(const Flags& a_Flags)
{
    uint32_t flags = 0u;
    if ((a_Flags & ResizableBits) != 0)
        flags |= SDL_WINDOW_RESIZABLE;
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

Impl::Impl(const Renderer::Handle& a_Renderer, const CreateWindowInfo& a_Info)
    : _sdlWindow(CreateSDLWindow(a_Info))
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
    Renderer::CreateSwapChainInfo swapChainInfo;
    swapChainInfo.vSync                     = _vSync;
    swapChainInfo.width                     = _width;
    swapChainInfo.height                    = _height;
    swapChainInfo.imageCount                = 3;
    swapChainInfo.windowInfo.setPixelFormat = true;
#ifdef _WIN32
    swapChainInfo.windowInfo.nativeDisplayHandle = wmInfo.info.win.hdc;
    swapChainInfo.windowInfo.nativeWindowHandle  = wmInfo.info.win.window;
#endif
    if (_swapChain == nullptr)
        _swapChain = Renderer::SwapChain::Create(_renderer, swapChainInfo);
    else
        _swapChain = Renderer::SwapChain::Recreate(_swapChain, swapChainInfo);
}

void Impl::Show() const
{
    SDL_ShowWindow(_sdlWindow);
}

void Impl::Present(const Renderer::RenderBuffer::Handle& a_RenderBuffer)
{
    Renderer::SwapChain::Present(_swapChain, a_RenderBuffer);
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

Renderer::SwapChain::Handle Impl::GetSwapChain() const
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
        _ResizeCallback(_width, _height);
    } break;
    case SDL_WINDOWEVENT_SIZE_CHANGED: {
        eventPtr = (Event*)new EventWindowSizeChanged(shared_from_this(), a_Event.data1, a_Event.data2);
        _ResizeCallback(_width, _height);
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

TabGraph::Window::Handle TabGraph::Window::Create(const Renderer::Handle& a_Renderer, const CreateWindowInfo& a_Info)
{
    return Handle(new TabGraph::Window::Impl(a_Renderer, a_Info));
}

void TabGraph::Window::Show(const Handle& a_Window)
{
    return a_Window->Show();
}

void TabGraph::Window::Present(const Handle& a_Window, const Renderer::RenderBuffer::Handle& a_RenderBuffer)
{
    return a_Window->Present(a_RenderBuffer);
}

bool TabGraph::Window::IsClosing(const Handle& a_Window)
{
    return a_Window->IsClosing();
}

uint32_t TabGraph::Window::GetWidth(const Handle& a_Window)
{
    return a_Window->GetWidth();
}

uint32_t TabGraph::Window::GetHeight(const Handle& a_Window)
{
    return a_Window->GetHeight();
}

TabGraph::Renderer::SwapChain::Handle TabGraph::Window::GetSwapChain(const Handle& a_Window)
{
    return a_Window->GetSwapChain();
}

std::any TabGraph::Window::GetNativeWindowHandle(const Handle& a_Window)
{
    SDL_SysWMinfo info;
    a_Window->GetWMInfo(info);
#ifdef _WIN32
    return HWND(info.info.win.window);
#elif defined __linux
    return info.info.x11.window;
#endif
}

std::any TabGraph::Window::GetNativeDisplayHandle(const Handle& a_Window)
{
    SDL_SysWMinfo info;
    a_Window->GetWMInfo(info);
#ifdef _WIN32
    return HDC(info.info.win.hdc);
#elif defined __linux
    return info.info.x11.display;
#endif
}
