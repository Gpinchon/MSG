#include <Events/EventManager.hpp>
#include <Events/SDL2/EventManager.hpp>
#include <Mouse/Events.hpp>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

TABGRAPH_REGISTER_EVENT(TabGraph::EventMouseButtonPressed);

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

static ButtonDownEventListener buttonDownListener;
}
