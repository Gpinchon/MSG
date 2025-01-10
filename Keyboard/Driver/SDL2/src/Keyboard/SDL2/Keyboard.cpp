#include <Events/EventManager.hpp>
#include <Events/SDL2/EventManager.hpp>
#include <Keyboard/Events.hpp>
#include <Keyboard/Keyboard.hpp>
#include <Keyboard/Structs.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <iostream>
#include <unordered_set>

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
        : Events::EventListener(SDL_KEYDOWN)
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

State TabGraph::Keyboard::GetState()
{
    int32_t nbr;
    auto SDL_state = SDL_GetKeyboardState(&nbr);
    State state;
    for (int32_t i = 0; i < nbr; i++)
        state.keys.at(i) = SDL_state[i];
    state.modifiers = GetModifiers();
    return state;
}

bool TabGraph::Keyboard::GetKeyState(const ScanCode& a_Scancode)
{
    return SDL_GetKeyboardState(nullptr)[size_t(a_Scancode)];
}

Modifiers TabGraph::Keyboard::GetModifiers()
{
    Modifiers modifiers = ModifierNoneBits;
    auto SDL_modifiers  = SDL_GetModState();
    if (SDL_modifiers & KMOD_LSHIFT)
        modifiers |= ModifierLShiftBits;
    if (SDL_modifiers & KMOD_RSHIFT)
        modifiers |= ModifierRShiftBits;
    if (SDL_modifiers & KMOD_LCTRL)
        modifiers |= ModifierLCtrlBits;
    if (SDL_modifiers & KMOD_RCTRL)
        modifiers |= ModifierRCtrlBits;
    if (SDL_modifiers & KMOD_LALT)
        modifiers |= ModifierLAltBits;
    if (SDL_modifiers & KMOD_RALT)
        modifiers |= ModifierRAltBits;
    if (SDL_modifiers & KMOD_LGUI)
        modifiers |= ModifierLGuiBits;
    if (SDL_modifiers & KMOD_RGUI)
        modifiers |= ModifierRGuiBits;
    if (SDL_modifiers & KMOD_NUM)
        modifiers |= ModifierNumBits;
    if (SDL_modifiers & KMOD_CAPS)
        modifiers |= ModifierCapsBits;
    if (SDL_modifiers & KMOD_MODE)
        modifiers |= ModifierModeBits;
    if (SDL_modifiers & KMOD_SCROLL)
        modifiers |= ModifierScrollBits;
    return modifiers;
}

Keycode GetKeycode(const ScanCode& a_Scancode)
{
    return Keycode(SDL_GetKeyFromScancode(SDL_Scancode(a_Scancode)));
}

ScanCode GetScancode(const Keycode& a_Keycode)
{
    return ScanCode(SDL_GetScancodeFromKey(SDL_Keycode(a_Keycode)));
}
}
