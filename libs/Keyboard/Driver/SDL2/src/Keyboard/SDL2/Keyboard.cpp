#include <MSG/Keyboard/Keyboard.hpp>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

Msg::Keyboard::State Msg::Keyboard::GetState()
{
    int32_t nbr;
    auto SDL_state = SDL_GetKeyboardState(&nbr);
    State state;
    for (int32_t i = 0; i < nbr; i++)
        state.keys.at(i) = SDL_state[i];
    state.modifiers = GetModifiers();
    return state;
}

bool Msg::Keyboard::GetKeyState(const ScanCode& a_Scancode)
{
    return SDL_GetKeyboardState(nullptr)[size_t(a_Scancode)];
}

Msg::Keyboard::Modifiers Msg::Keyboard::GetModifiers()
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

Msg::Keyboard::KeyCode Msg::Keyboard::GetKeycode(const ScanCode& a_Scancode)
{
    return KeyCode(SDL_GetKeyFromScancode(SDL_Scancode(a_Scancode)));
}

Msg::Keyboard::ScanCode Msg::Keyboard::GetScancode(const KeyCode& a_Keycode)
{
    return ScanCode(SDL_GetScancodeFromKey(SDL_Keycode(a_Keycode)));
}
