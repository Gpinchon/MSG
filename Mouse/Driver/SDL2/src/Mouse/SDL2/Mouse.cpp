#include <Mouse/Mouse.hpp>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

namespace TabGraph::Mouse {
State TabGraph::Mouse::GetState()
{
    State state;
    state.buttons = SDL_GetMouseState(&state.position.x, &state.position.y);
    return state;
}
}
