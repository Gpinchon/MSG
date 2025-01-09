#pragma once
#include <Renderer/Handles.hpp>

#include <memory>

typedef struct SDL_Window SDL_Window;
typedef struct SDL_SysWMinfo SDL_SysWMinfo;
typedef struct SDL_WindowEvent SDL_WindowEvent;

namespace TabGraph::Window {
struct CreateWindowInfo;
class EventListener;
}

namespace TabGraph::Window {
class Impl : public std::enable_shared_from_this<Impl> {
public:
    Impl(const Renderer::Handle& a_Renderer, const CreateWindowInfo& a_Info);
    ~Impl();
    void HandleEvent(const SDL_WindowEvent& a_Event);
    void Show() const;
    void Present(const Renderer::RenderBuffer::Handle& a_RenderBuffer);
    bool IsClosing() const;
    unsigned GetID() const;
    int32_t GetWidth() const;
    int32_t GetHeight() const;
    Renderer::SwapChain::Handle GetSwapChain() const;
    void GetWMInfo(SDL_SysWMinfo& a_Info) const;

private:
    void _ResizeCallback(const uint32_t& a_Width, const uint32_t& a_Height);
    SDL_Window* _sdlWindow = nullptr;
    bool _vSync            = false;
    bool _isClosing        = false;
    int32_t _width;
    int32_t _height;
    std::shared_ptr<EventListener> _eventListener;
    Renderer::Handle _renderer;
    Renderer::SwapChain::Handle _swapChain;
};
}
