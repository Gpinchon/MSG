/*
* @Author: gpinchon
* @Date:   2021-05-31 12:11:49
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-31 12:11:49
*/

#include <Driver/OpenGL/Renderer/Context.hpp>

#include <stdexcept>

#if MEDIALIBRARY == SDL2
#include <Driver/SDL2/Window.hpp>
#include <SDL_video.h>
#endif//MEDIALIBRARY == SDL2

namespace OpenGL {
Context::Context(std::weak_ptr<TabGraph::Core::Window> window, uint8_t versionMajor, uint8_t versionMinor)
    : _window(window)
{
#if MEDIALIBRARY == SDL2
    //SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    //SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    //SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, versionMajor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, versionMinor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    _handle = SDL_GL_CreateContext((SDL_Window*)GetWindow()->GetImpl().GetHandle());
    if (_handle == nullptr) {
        throw std::runtime_error(SDL_GetError());
    }
#endif//MEDIALIBRARY == SDL2
}
Context::~Context()
{
#if MEDIALIBRARY == SDL2
    SDL_GL_DeleteContext(GetHandle());
#endif//MEDIALIBRARY == SDL2
}
void Context::MakeCurrent()
{
#if MEDIALIBRARY == SDL2
    SDL_GL_MakeCurrent((SDL_Window*)GetWindow()->GetImpl().GetHandle(), GetHandle());
#endif//MEDIALIBRARY == SDL2
}
};

