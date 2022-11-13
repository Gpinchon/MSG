/*
* @Author: gpinchon
* @Date:   2021-05-19 23:30:21
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-20 12:13:52
*/

#include <Driver/SDL2/Events/Manager.hpp>
#include <Driver/SDL2/Events/Event.hpp>
#include <Engine.hpp>

#include <SDL.h>
#include <SDL_events.h>

namespace TabGraph::Events {
Manager::Impl::Impl()
{
    if (!SDL_WasInit(SDL_INIT_EVENTS) && SDL_Init(SDL_INIT_EVENTS) < 0)
        throw std::runtime_error(SDL_GetError());
}

void Manager::Impl::PollEvents()
{
    SDL_Event SDLevent;
    _lock.lock();
    while (SDL_PollEvent(&SDLevent))
        _eventsQueue.push(SDL2::CreateEvent(&SDLevent));
    _lock.unlock();
    while (!_eventsQueue.empty()) {
        _lock.lock();
        auto event{ std::move(_eventsQueue.front()) };
        _eventsQueue.pop();
        _lock.unlock();
        On(event.type)(event);
    }
}
}