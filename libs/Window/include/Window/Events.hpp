#pragma once

#include <Events/RegisteredEvent.hpp>
#include <Window/Handles.hpp>

namespace MSG {
template <typename EventName>
class EventWindow : public RegisteredEvent<EventName> {
public:
    EventWindow(const Window::Handle& a_Window)
        : RegisteredEvent<EventName>()
        , window(a_Window)
    {
    }
    virtual ~EventWindow() = default;
    const Window::Handle window;
};

#define DECLARE_WINDOW_EVENT(EventName)               \
    class EventName : public EventWindow<EventName> { \
    public:                                           \
        using EventWindow::EventWindow;               \
        virtual ~EventName() = default;               \
    };

DECLARE_WINDOW_EVENT(EventWindowClosed);
DECLARE_WINDOW_EVENT(EventWindowEntered);
DECLARE_WINDOW_EVENT(EventWindowExposed);
DECLARE_WINDOW_EVENT(EventWindowFocusGained);
DECLARE_WINDOW_EVENT(EventWindowFocusLost);
DECLARE_WINDOW_EVENT(EventWindowHidden);
DECLARE_WINDOW_EVENT(EventWindowLeft);
DECLARE_WINDOW_EVENT(EventWindowMaximized);
DECLARE_WINDOW_EVENT(EventWindowMinimized);
DECLARE_WINDOW_EVENT(EventWindowRestored);
DECLARE_WINDOW_EVENT(EventWindowShown);
DECLARE_WINDOW_EVENT(EventWindowTookFocus);

class EventWindowResized : public EventWindow<EventWindowResized> {
public:
    EventWindowResized(const Window::Handle& a_Window, const uint32_t& a_Width, const uint32_t& a_Height)
        : EventWindow(a_Window)
        , width(a_Width)
        , height(a_Height)
    {
    }
    virtual ~EventWindowResized() = default;
    const uint32_t width;
    const uint32_t height;
};

class EventWindowSizeChanged : public EventWindow<EventWindowSizeChanged> {
public:
    EventWindowSizeChanged(const Window::Handle& a_Window, const uint32_t& a_Width, const uint32_t& a_Height)
        : EventWindow(a_Window)
        , width(a_Width)
        , height(a_Height)
    {
    }
    virtual ~EventWindowSizeChanged() = default;
    const uint32_t width;
    const uint32_t height;
};

class EventWindowMoved : public EventWindow<EventWindowMoved> {
public:
    EventWindowMoved(const Window::Handle& a_Window, const uint32_t& a_Left, const uint32_t& a_Top)
        : EventWindow(a_Window)
        , left(a_Left)
        , top(a_Top)
    {
    }
    virtual ~EventWindowMoved() = default;
    const uint32_t left;
    const uint32_t top;
};
}
