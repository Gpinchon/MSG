#include <Events/EventManager.hpp>
#include <Events/SDL2/EventManager.hpp>
#include <Keyboard/Events.hpp>

TABGRAPH_REGISTER_EVENT(TabGraph::EventKeyboardKeyPressed);
TABGRAPH_REGISTER_EVENT(TabGraph::EventKeyboardKeyReleased);

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

namespace TabGraph::Keyboard {
class KeyDownEventListener : Events::EventListener {
public:
    KeyDownEventListener()
        : Events::EventListener(SDL_KEYDOWN)
    {
    }
    void operator()(const SDL_Event& a_Event, std::any a_UserData) override
    {
        auto& keyEvent = a_Event.key;
        auto eventPtr  = (Event*)new EventKeyboardKeyPressed(
            keyEvent.windowID,
            ScanCode(keyEvent.keysym.scancode),
            keyEvent.repeat);
        auto uniquePtr = std::unique_ptr<Event>(eventPtr);
        Events::PushNoLock(uniquePtr);
    }
};

class KeyUpEventListener : Events::EventListener {
public:
    KeyUpEventListener()
        : Events::EventListener(SDL_KEYUP)
    {
    }
    void operator()(const SDL_Event& a_Event, std::any a_UserData) override
    {
        auto& keyEvent = a_Event.key;
        auto eventPtr  = (Event*)new EventKeyboardKeyReleased(
            keyEvent.windowID,
            ScanCode(keyEvent.keysym.scancode),
            keyEvent.repeat);
        auto uniquePtr = std::unique_ptr<Event>(eventPtr);
        Events::PushNoLock(uniquePtr);
    }
};

static KeyDownEventListener keyDownListener;
static KeyUpEventListener keyUpListener;
}
