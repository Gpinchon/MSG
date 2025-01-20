#pragma once

#include <Events/EventManager.hpp>

namespace MSG {
template <typename EventType>
class RegisteredEvent : Event {
public:
    RegisteredEvent()
        : Event(Type)
    {
    }
    static EventTypeID Type;
};
}

#define MSG_REGISTER_EVENT(EventType) \
    template <>                       \
    MSG::EventTypeID MSG::RegisteredEvent<EventType>::Type = MSG::Events::RegisterType(MSG::EventTypeID(typeid(EventType).hash_code()));
