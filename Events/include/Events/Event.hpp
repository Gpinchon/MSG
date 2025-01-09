#pragma once

#include <Tools/StrongTypedef.hpp>

#include <cstdint>
#include <limits>

namespace TabGraph {
TABGRAPH_STRONG_TYPEDEF(EventTypeID, uint64_t);
static constexpr auto EventTypeNone = std::numeric_limits<EventTypeID>::max();
class Event {
public:
    Event(const EventTypeID& a_TypeID = EventTypeNone)
        : typeID(a_TypeID)
    {
    }
    virtual ~Event()   = 0;
    EventTypeID typeID = EventTypeNone;
};
}
