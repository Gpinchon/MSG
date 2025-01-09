#pragma once

#include <Tools/StrongTypedef.hpp>

#include <cstdint>
#include <limits>

namespace TabGraph {
TABGRAPH_STRONG_TYPEDEF(EventTypeID, uint64_t);
static constexpr auto EventTypeNone = EventTypeID(std::numeric_limits<EventTypeID::type>::max());
class Event {
public:
    Event(const EventTypeID& a_TypeID = EventTypeNone)
        : typeID(a_TypeID)
    {
    }
    virtual ~Event()   = default;
    EventTypeID typeID = EventTypeNone;
};
}
