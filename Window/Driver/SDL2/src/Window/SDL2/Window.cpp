#include <Events/EventManager.hpp>
#include <Window/SDL2/Window.hpp>
#include <Window/Structs.hpp>
#include <Window/Window.hpp>

#include <SDL2/SDL.h>

namespace TabGraph::Window {
SDL_WindowFlags ConvertFlags(const Flags& a_Flags)
{
    uint32_t flags = 0u;
    if ((a_Flags & ResizableBits) != 0)
        flags |= SDL_WINDOW_RESIZABLE;
    return SDL_WindowFlags(flags);
}

static int WindowEventWatch(void* a_UserData, SDL_Event* a_Event)
{

    if (a_Event->type == SDL_WINDOWEVENT) {
        Event* event;
        auto ptr = std::unique_ptr<Event>(event);
        Events::Push(ptr);
    }
}

Impl::Impl(const CreateWindowInfo& a_Info)
    : _sdlWindow(SDL_CreateWindow(
          a_Info.name.c_str(),
          a_Info.positionX == -1 ? SDL_WINDOWPOS_CENTERED : a_Info.positionX,
          a_Info.positionY == -1 ? SDL_WINDOWPOS_CENTERED : a_Info.positionY,
          a_Info.width, a_Info.height,
          ConvertFlags(a_Info.flags))) // no flags because we want to set the pixel format ourselves
{
    SDL_AddEventWatch(WindowEventWatch, this);
}

Impl::~Impl()
{
    SDL_DelEventWatch(WindowEventWatch, this);
}
}

TabGraph::Window::Handle TabGraph::Window::Create(const CreateWindowInfo& a_Info)
{
    return Handle(new TabGraph::Window::Impl(a_Info));
}
