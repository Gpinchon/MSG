#pragma once

#include <Renderer/Handle.hpp>

#include <string>

TABGRAPH_RENDERER_HANDLE(TabGraph::Renderer);
TABGRAPH_RENDERER_HANDLE(TabGraph::Renderer::RenderBuffer);
TABGRAPH_RENDERER_HANDLE(TabGraph::Renderer::Window);

namespace TabGraph::SG {
class Scene;
}

namespace TabGraph::Renderer {
struct Info {
    std::string name{ "" };
    uint32_t applicationVersion{ 0 };
};
/** @return an instance of Renderer */
Handle Create(const Info& a_Info);

/**
* @brief loads the necessary data for rendering
*/
void Load(
    const Handle& a_Renderer,
    const SG::Scene& a_Scene);

/**
* @brief loads the necessary data for rendering for the specified entity
*/
void Load(
    const Handle& a_Renderer,
    const uint32_t a_EntityID);

/**
* @brief unloads the rendering data
*/
void Unload(
    const Handle& a_Renderer,
    const SG::Scene& a_Scene);

/**
* @brief unloads the rendering data of the specified entity
*/
void Unload(
    const Handle& a_Renderer,
    const uint32_t a_EntityID);

/**
* @arg a_Renderer : the renderer to use for rendering
* @arg a_Scene    : the scene to render
* @arg a_Buffer   : the render buffer to render to
*/
void Render(
    const Handle& a_Renderer,
    const SG::Scene& a_Scene,
    const RenderBuffer::Handle& a_Buffer);

/**
* @brief Updates the renderer, cleaning up stuff if needed
*/
void Update(const Handle& a_Renderer);
}