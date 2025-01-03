#pragma once

#include <Events/EventManager.hpp>

namespace TabGraph {
template<typename EventType>
class RegisteredEvent : Event {
public:
    RegisteredEvent(const EventTypeID& a_Hint = EventTypeNone) : Event(a_Hint) { };
    static EventTypeID Type = EventTypeNone;
private:
    EventTypeID _RegisterType(const EventTypeID& a_Hint = EventTypeNone) {
        if (Type == EventTypeNone)
            Type = Events::RegisterType(a_Hint);
        return Type;
    }
};
}