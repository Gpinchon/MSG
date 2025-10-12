#pragma once

#include <MSG/Events/RegisteredEvent.hpp>
#include <MSG/Keyboard/Enums.hpp>

namespace Msg {
template <typename EventName>
class EventKeyboardKey : public RegisteredEvent<EventName> {
public:
    EventKeyboardKey(
        const uint32_t& a_WindowID,
        const Keyboard::ScanCode& a_Scancode,
        const bool& a_Repeat)
        : RegisteredEvent<EventName>()
        , windowID(a_WindowID)
        , scancode(a_Scancode)
        , repeat(a_Repeat)
    {
    }
    virtual ~EventKeyboardKey() = default;
    const uint32_t windowID;
    const Keyboard::ScanCode scancode;
    const bool repeat;
};

#define DECLARE_KEYBOARD_EVENT(EventName)                  \
    class EventName : public EventKeyboardKey<EventName> { \
    public:                                                \
        using EventKeyboardKey::EventKeyboardKey;          \
        virtual ~EventName() = default;                    \
    };

DECLARE_KEYBOARD_EVENT(EventKeyboardKeyPressed);
DECLARE_KEYBOARD_EVENT(EventKeyboardKeyReleased);
}
