#pragma once

typedef struct SDL_Window SDL_Window;

namespace TabGraph::Window {
struct CreateWindowInfo;
}

namespace TabGraph::Window {
class Impl {
public:
    Impl(const CreateWindowInfo& a_Info);
    ~Impl();

private:
    SDL_Window* _sdlWindow;
};
}