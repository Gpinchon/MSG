#pragma once

#include <MSG/Events/EventManager.hpp>

namespace Msg {
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
    Msg::EventTypeID Msg::RegisteredEvent<EventType>::Type = Msg::Events::RegisterType(Msg::EventTypeID(typeid(EventType).hash_code()));
