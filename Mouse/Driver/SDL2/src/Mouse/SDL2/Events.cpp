#include <Events/EventManager.hpp>
#include <Events/SDL2/EventManager.hpp>
#include <Mouse/Events.hpp>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

TABGRAPH_REGISTER_EVENT(TabGraph::EventMouseButtonPressed);
TABGRAPH_REGISTER_EVENT(TabGraph::EventMouseButtonReleased);
TABGRAPH_REGISTER_EVENT(TabGraph::EventMouseMotion);
TABGRAPH_REGISTER_EVENT(TabGraph::EventMouseWheel);

namespace TabGraph::Events {
class ButtonDownEventListener : Events::EventListener {
public:
    ButtonDownEventListener()
        : Events::EventListener(SDL_MOUSEBUTTONDOWN)
    {
    }
    void operator()(const SDL_Event& a_Event, std::any a_UserData) override
    {
        auto& event   = a_Event.button;
        auto eventPtr = (Event*)new EventMouseButtonPressed(
            event.windowID,
            event.which,
            { event.x, event.y },
            event.button,
            event.clicks);
        auto uniquePtr = std::unique_ptr<Event>(eventPtr);
        Events::PushNoLock(uniquePtr);
    }
};

class ButtonUpEventListener : Events::EventListener {
public:
    ButtonUpEventListener()
        : Events::EventListener(SDL_MOUSEBUTTONUP)
    {
    }
    void operator()(const SDL_Event& a_Event, std::any a_UserData) override
    {
        auto& event   = a_Event.button;
        auto eventPtr = (Event*)new EventMouseButtonReleased(
            event.windowID,
            event.which,
            { event.x, event.y },
            event.button,
            event.clicks);
        auto uniquePtr = std::unique_ptr<Event>(eventPtr);
        Events::PushNoLock(uniquePtr);
    }
};

class MotionEventListener : Events::EventListener {
public:
    MotionEventListener()
        : Events::EventListener(SDL_MOUSEMOTION)
    {
    }
    void operator()(const SDL_Event& a_Event, std::any a_UserData) override
    {
        auto& event   = a_Event.motion;
        auto eventPtr = (Event*)new EventMouseMotion(
            event.windowID,
            event.which,
            { event.x, event.y },
            { event.xrel, event.yrel });
        auto uniquePtr = std::unique_ptr<Event>(eventPtr);
        Events::PushNoLock(uniquePtr);
    }
};

class WheelEventListener : Events::EventListener {
public:
    WheelEventListener()
        : Events::EventListener(SDL_MOUSEWHEEL)
    {
    }
    void operator()(const SDL_Event& a_Event, std::any a_UserData) override
    {
        auto& event   = a_Event.wheel;
        auto eventPtr = (Event*)new EventMouseWheel(
            event.windowID,
            event.which,
            { event.mouseX, event.mouseY },
            { event.x, event.y },
            { event.preciseX, event.preciseY },
            event.direction == SDL_MOUSEWHEEL_FLIPPED ? Mouse::WeelDirection::Flipped : Mouse::WeelDirection::Normal);
        auto uniquePtr = std::unique_ptr<Event>(eventPtr);
        Events::PushNoLock(uniquePtr);
    }
};

static ButtonDownEventListener buttonDownListener;
static ButtonUpEventListener buttonUpListener;
static MotionEventListener motionListener;
static WheelEventListener wheelListener;
}
