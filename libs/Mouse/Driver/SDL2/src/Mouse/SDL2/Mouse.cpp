#include <MSG/Mouse/Mouse.hpp>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

Msg::Mouse::State Msg::Mouse::GetState()
{
    State state;
    state.buttons = SDL_GetMouseState(&state.position.x, &state.position.y);
    return state;
}
