/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:12:51
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Events/Signal.hpp>

#include <filesystem>
#include <string> // for string
#include <sys/types.h> // for int8_t

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph {
namespace Renderer {
struct FrameRenderer;
}
namespace Nodes {
class Scene;
}
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Core {
/** @brief This class orchestrates the rendering loop and logics around it */
class Engine {
public:
    Engine(std::shared_ptr<Renderer::FrameRenderer> frameRenderer);
    ~Engine();
    void SetCurrentScene(std::shared_ptr<Nodes::Scene>);
    std::shared_ptr<Nodes::Scene> GetCurrentScene() const;
    void SetFrameRenderer(std::shared_ptr<Renderer::FrameRenderer>);
    std::shared_ptr<Renderer::FrameRenderer> GetFrameRenderer() const;
    /** @brief Starts Engine loop */
    void Start(void);
    /** @brief Tells the Engine to stop the loop */
    void Stop(void);
    /** @brief Called 60 times per seconds at best */
    Events::Signal<float>& OnFixedUpdate();
    /** @brief Called on every loop */
    Events::Signal<float>& OnUpdate();

    /** @return See https://wiki.libsdl.org/SDL_GetBasePath */
    static const std::filesystem::path GetProgramPath();
    /** @return std::filesystem::current_path() */
    static const std::filesystem::path GetExecutionPath();
    /** @return Program path + "/res/" */
    static const std::filesystem::path GetResourcePath(void);

private:
    class Impl;
    friend Impl;
    std::unique_ptr<Impl> _impl;
};

}
