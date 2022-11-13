/*
* @Author: gpinchon
* @Date:   2021-03-25 11:04:12
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-28 08:05:14
*/

#include <Renderer/SceneRenderer.hpp>
#include <Nodes/Scene.hpp>

#if RENDERINGAPI == OpenGL
#include <Driver/OpenGL/Renderer/SceneRenderer.hpp>
#endif

#include <glm/glm.hpp>

#include <map>

namespace TabGraph::Renderer {
static std::map<std::weak_ptr<Nodes::Scene>, Renderer::SceneRenderer, std::owner_less<>> s_sceneRenderers;
Renderer::SceneRenderer& GetRenderer(std::shared_ptr<Nodes::Scene> scene) {
    auto renderer{ s_sceneRenderers.find(scene) };
    s_sceneRenderers.try_emplace(scene, *scene);
    return s_sceneRenderers.at(scene);
}

void CleanupRenderers() {
    for (auto it = s_sceneRenderers.begin(); it != s_sceneRenderers.end();) {
        if (it->first.lock() == nullptr)
            s_sceneRenderers.erase(it++);
        else ++it;
    }
}

void OnFrameBegin(std::shared_ptr<Nodes::Scene> scene, const Options& options)
{
    CleanupRenderers();
    GetRenderer(scene).OnFrameBegin(options);
}

void Render(std::shared_ptr<Nodes::Scene> scene, const Options& options)
{
    GetRenderer(scene).Render(options, glm::mat4(1));
}

void Render(std::shared_ptr<Nodes::Scene> scene, const Options& options, const glm::mat4& rootMatrix)
{
    GetRenderer(scene).Render(options, rootMatrix);
}

void OnFrameEnd(std::shared_ptr<Nodes::Scene> scene, const Options& options)
{
    GetRenderer(scene).OnFrameEnd(options);
}
};
