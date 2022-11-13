/*
* @Author: gpinchon
* @Date:   2021-05-24 13:38:16
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:12:52
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Engine.hpp>
#include <Events/Signal.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Events {
struct Event;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Core {
struct Engine::Impl : Events::Trackable {
    Impl(std::shared_ptr<Renderer::FrameRenderer> frameRenderer);
    void Start();
    inline auto Stop()
    {
        _loop = false;
    }
    inline auto SetCurrentScene(std::shared_ptr<Nodes::Scene> scene)
    {
        _currentScene = scene;
    }
    inline auto GetCurrentScene() const
    {
        return _currentScene;
    }
    inline auto SetFrameRenderer(std::shared_ptr<Renderer::FrameRenderer> frameRenderer)
    {
        _frameRenderer = frameRenderer;
    }
    inline auto GetFrameRenderer() const
    {
        return _frameRenderer;
    }
    inline auto& OnFixedUpdate()
    {
        return _onFixedUpdate;
    }
    inline auto& OnUpdate()
    {
        return _onUpdate;
    }
    static std::filesystem::path GetProgramPath();
    static std::filesystem::path GetExecutionPath();

private:
    std::atomic<bool> _loop { true };
    Events::Signal<float> _onFixedUpdate;
    Events::Signal<float> _onUpdate;
    Events::Signal<const Events::Event&>::ScoppedSlot _quitSlot;
    std::shared_ptr<Renderer::FrameRenderer> _frameRenderer;
    std::shared_ptr<Nodes::Scene> _currentScene;
    void _ProcessEvent(const Events::Event&);
};
}
