#pragma once

#include <Events/EventManager.hpp>

namespace TabGraph {
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

#define TABGRAPH_REGISTER_EVENT(EventType) \
    template <>                            \
    TabGraph::EventTypeID TabGraph::RegisteredEvent<EventType>::Type = TabGraph::Events::RegisterType(TabGraph::EventTypeID(typeid(EventType).hash_code()));
