#pragma once

#include <any>
#include <functional>

union SDL_Event;
enum SDL_EventType;

namespace TabGraph::Events {
using EventListener = std::function<void(const SDL_Event&, std::any)>;
// Set an event listener that will be called on Pump when an even of specified Type is detected
void SetEventListener(const SDL_EventType& a_EventType, const EventListener& a_Listener, std::any a_UserData = {});
// Remove the event listener for the specified event type
void RemoveEventListener(const SDL_EventType& a_EventType);
}