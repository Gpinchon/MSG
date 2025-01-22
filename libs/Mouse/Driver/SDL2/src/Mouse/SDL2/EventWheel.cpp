#include <MSG/Events/EventManager.hpp>
#include <MSG/Events/SDL2/EventManager.hpp>
#include <MSG/Mouse/Events.hpp>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

MSG_REGISTER_EVENT(MSG::EventMouseWheel);

namespace MSG::Events {
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

static WheelEventListener wheelListener;
}
