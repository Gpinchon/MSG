/*
* @Author: gpinchon
* @Date:   2021-05-24 13:38:06
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:12:48
*/

#include <Driver/SDL2/Engine.hpp>
#include <Events/Manager.hpp>
#include <Renderer/FrameRenderer.hpp>
#include <Renderer/Renderer.hpp>

#include <SDL_filesystem.h>
#include <SDL_timer.h>
#include <SDL_video.h>

namespace TabGraph::Core {
Engine::Impl::Impl(std::shared_ptr<Renderer::FrameRenderer> frameRenderer)
    : _frameRenderer(frameRenderer)
{
    _quitSlot = Events::Manager::On(Events::Event::Type::Quit).ConnectMember(this, &Engine::Impl::_ProcessEvent);
}

std::filesystem::path Engine::Impl::GetProgramPath()
{
    return std::filesystem::absolute(SDL_GetBasePath()).parent_path();
}

std::filesystem::path Engine::Impl::GetExecutionPath()
{
    return std::filesystem::current_path();
}

void Engine::Impl::_ProcessEvent(const Events::Event&)
{
    Stop();
}

void Engine::Impl::Start()
{
    double ticks;
    double lastTicks;
    double fixedTiming = lastTicks = SDL_GetTicks() / 1000.f;

    while (_loop) {
        ticks = SDL_GetTicks() / 1000.0;
        _onUpdate(ticks - lastTicks);
        if (ticks - fixedTiming >= 0.015) {
            Events::Manager::PollEvents();
            _onFixedUpdate(ticks - fixedTiming);
            fixedTiming = ticks;
        }
        _frameRenderer->RenderFrame(_currentScene);
        lastTicks = ticks;
    }
}
}
