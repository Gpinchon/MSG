/*
* @Author: gpinchon
* @Date:   2021-05-19 23:28:33
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:12:53
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Events/Event.hpp>
#include <Events/Manager.hpp>
#include <Events/Signal.hpp>

#include <array>
#include <mutex>
#include <queue>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////
union SDL_Event;

////////////////////////////////////////////////////////////////////////////////
// Class Definitions
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Events {
class Manager::Impl {
public:
    Impl();
    inline Signal<const Event&>& On(Event::Type type)
    {
        return _onEvent.at(size_t(type));
    }
    inline void PushEvent(const Event& event)
    {
        std::unique_lock lock(_lock);
        _eventsQueue.push(event);
    }
    void PollEvents();

private:
    std::array<Signal<const Event&>, size_t(Event::Type::MaxValue)> _onEvent;
    std::queue<Event> _eventsQueue;
    std::mutex _lock;
};
}
