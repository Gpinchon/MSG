#pragma once

#include <Events/RegisteredEvent.hpp>
#include <Window/Handles.hpp>

namespace TabGraph {
class EventWindow {
public:
    EventWindow(const Window::Handle& a_Window)
        : window(a_Window)
    {
    }
    const Window::Handle window;
};

#define DECLARE_WINDOW_EVENT(EventName)                                       \
    class EventName : public EventWindow, public RegisteredEvent<EventName> { \
    public:                                                                   \
        EventName(const Window::Handle& a_Window)                             \
            : EventWindow(a_Window)                                           \
            , RegisteredEvent()                                               \
        {                                                                     \
        }                                                                     \
    };

DECLARE_WINDOW_EVENT(EventWindowClosed);
DECLARE_WINDOW_EVENT(EventWindowEntered);
DECLARE_WINDOW_EVENT(EventWindowExposed);
DECLARE_WINDOW_EVENT(EventWindowFocusGained);
DECLARE_WINDOW_EVENT(EventWindowFocusLost);
DECLARE_WINDOW_EVENT(EventWindowLeft);
DECLARE_WINDOW_EVENT(EventWindowMaximized);
DECLARE_WINDOW_EVENT(EventWindowMinized);
DECLARE_WINDOW_EVENT(EventWindowRestored);
DECLARE_WINDOW_EVENT(EventWindowShown);

class EventWindowResized : public EventWindow, public RegisteredEvent<EventWindowResized> {
public:
    EventWindowResized(const Window::Handle& a_Window, const uint32_t& a_Width, const uint32_t& a_Height)
        : EventWindow(a_Window)
        , RegisteredEvent()
        , width(a_Width)
        , height(a_Height)
    {
    }
    const uint32_t width;
    const uint32_t height;
};

class EventWindowMoved : public EventWindow, public RegisteredEvent<EventWindowMoved> {
public:
    EventWindowMoved(const Window::Handle& a_Window, const uint32_t& a_Left, const uint32_t& a_Top)
        : EventWindow(a_Window)
        , RegisteredEvent()
        , left(a_Left)
        , top(a_Top)
    {
    }
    const uint32_t left;
    const uint32_t top;
};
}
