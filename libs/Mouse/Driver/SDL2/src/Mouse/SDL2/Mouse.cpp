#include <MSG/Mouse/Mouse.hpp>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

MSG::Mouse::State MSG::Mouse::GetState()
{
    State state;
    state.buttons = SDL_GetMouseState(&state.position.x, &state.position.y);
    return state;
}
