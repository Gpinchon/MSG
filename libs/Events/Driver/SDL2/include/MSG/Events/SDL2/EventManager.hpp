#pragma once

#include <any>
#include <functional>

union SDL_Event;

namespace MSG::Events {
using EventListenerCallback = std::function<void(const SDL_Event&, std::any)>;
// Set an event listener that will be called on Pump when an even of specified Type is detected
void SetEventListener(const uint32_t& a_EventType, const EventListenerCallback& a_Listener, std::any a_UserData = {});
// Remove the event listener for the specified event type
void RemoveEventListener(const uint32_t& a_EventType);
class EventListener {
public:
    EventListener(const uint32_t& a_EventType, std::any a_UserData = {})
        : eventType(a_EventType)
    {
        SetEventListener(eventType, std::bind(&EventListener::operator(), this, std::placeholders::_1, std::placeholders::_2), a_UserData);
    }
    virtual ~EventListener() { RemoveEventListener(eventType); }
    const uint32_t eventType;
    virtual void operator()(const SDL_Event& a_Event, std::any a_UserData) = 0;
};
}
