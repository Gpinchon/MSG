#include <MSG/Keyboard/Keyboard.hpp>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

MSG::Keyboard::State MSG::Keyboard::GetState()
{
    int32_t nbr;
    auto SDL_state = SDL_GetKeyboardState(&nbr);
    State state;
    for (int32_t i = 0; i < nbr; i++)
        state.keys.at(i) = SDL_state[i];
    state.modifiers = GetModifiers();
    return state;
}

bool MSG::Keyboard::GetKeyState(const ScanCode& a_Scancode)
{
    return SDL_GetKeyboardState(nullptr)[size_t(a_Scancode)];
}

MSG::Keyboard::Modifiers MSG::Keyboard::GetModifiers()
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

MSG::Keyboard::KeyCode MSG::Keyboard::GetKeycode(const ScanCode& a_Scancode)
{
    return KeyCode(SDL_GetKeyFromScancode(SDL_Scancode(a_Scancode)));
}

MSG::Keyboard::ScanCode MSG::Keyboard::GetScancode(const KeyCode& a_Keycode)
{
    return ScanCode(SDL_GetScancodeFromKey(SDL_Keycode(a_Keycode)));
}
