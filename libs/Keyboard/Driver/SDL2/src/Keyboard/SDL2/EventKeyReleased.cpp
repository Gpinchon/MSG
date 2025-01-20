#include <Events/EventManager.hpp>
#include <Events/SDL2/EventManager.hpp>
#include <Keyboard/Events.hpp>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

MSG_REGISTER_EVENT(MSG::EventKeyboardKeyReleased);

namespace MSG::Keyboard {
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

static KeyUpEventListener keyUpListener;
}
