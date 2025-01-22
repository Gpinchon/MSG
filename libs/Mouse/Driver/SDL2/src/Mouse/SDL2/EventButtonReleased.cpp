#include <MSG/Events/EventManager.hpp>
#include <MSG/Events/SDL2/EventManager.hpp>
#include <MSG/Mouse/Events.hpp>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

MSG_REGISTER_EVENT(MSG::EventMouseButtonReleased);

namespace MSG::Events {
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

static ButtonUpEventListener buttonUpListener;
}
