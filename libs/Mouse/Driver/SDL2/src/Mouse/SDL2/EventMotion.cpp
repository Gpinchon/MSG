#include <Events/EventManager.hpp>
#include <Events/SDL2/EventManager.hpp>
#include <Mouse/Events.hpp>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

TABGRAPH_REGISTER_EVENT(TabGraph::EventMouseMotion);

namespace TabGraph::Events {
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

static MotionEventListener motionListener;
}
