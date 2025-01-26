#pragma once

#include <MSG/Renderer/Handles.hpp>
#include <MSG/SwapChain/Handles.hpp>

#include <memory>

typedef struct SDL_Window SDL_Window;
typedef struct SDL_SysWMinfo SDL_SysWMinfo;
typedef struct SDL_WindowEvent SDL_WindowEvent;

namespace MSG::Window {
struct CreateWindowInfo;
class EventListener;
}

namespace MSG::Window {
class Impl : public std::enable_shared_from_this<Impl> {
public:
    Impl(const Renderer::Handle& a_Renderer, const CreateWindowInfo& a_Info);
    ~Impl();
    void HandleEvent(const SDL_WindowEvent& a_Event);
    void WaitSwapChain() const;
    void Show() const;
    void Present(const Renderer::RenderBuffer::Handle& a_RenderBuffer);
    bool IsClosing() const;
    unsigned GetID() const;
    int32_t GetWidth() const;
    int32_t GetHeight() const;
    SwapChain::Handle GetSwapChain() const;
    void GetWMInfo(SDL_SysWMinfo& a_Info) const;
std::any GetNativeDisplayHandle() const;
    std::any GetNativeWindowHandle() const;

private:
    void _ResizeCallback(const uint32_t& a_Width, const uint32_t& a_Height);
    SDL_Window* _sdlWindow = nullptr;
    bool _vSync            = false;
    bool _isClosing        = false;
    int32_t _width;
    int32_t _height;
    std::shared_ptr<EventListener> _eventListener;
    Renderer::Handle _renderer;
    SwapChain::Handle _swapChain;
};
}
