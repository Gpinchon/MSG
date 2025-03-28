#pragma once

#include <MSG/Renderer/Handles.hpp>
#include <MSG/Renderer/Structs.hpp>

#include <string>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
class Scene;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace MSG::Renderer {
/** @return an instance of Renderer */
Handle Create(const CreateRendererInfo& a_Info, const RendererSettings& a_Settings);

/**
 * @brief Sets the current render buffer, an Update is adviced afterwards
 * @param a_Renderer the renderer to update
 * @param a_RenderBuffer the render buffer to set for next render
 */
void SetActiveRenderBuffer(
    const Handle& a_Renderer,
    const RenderBuffer::Handle& a_RenderBuffer);

/**
 * @return the current render buffer of the specified renderer
 * @param a_Renderer the renderer to query
 */
RenderBuffer::Handle GetActiveRenderBuffer(
    const Handle& a_Renderer);

void SetActiveScene(
    const Handle& a_Renderer,
    Scene* const a_Scene);

Scene* GetActiveScene(
    const Handle& a_Renderer);

/**
 * @brief loads the necessary data for rendering
 */
void Load(
    const Handle& a_Renderer,
    const Scene& a_Scene);

/**
 * @brief loads the necessary data for rendering for the specified entity
 */
void Load(
    const Handle& a_Renderer,
    const ECS::DefaultRegistry::EntityRefType& a_Entity);

/**
 * @brief unloads the rendering data
 */
void Unload(
    const Handle& a_Renderer,
    const Scene& a_Scene);

/**
 * @brief unloads the rendering data of the specified entity
 */
void Unload(
    const Handle& a_Renderer,
    const ECS::DefaultRegistry::EntityRefType& a_Entity);

/**
 * @brief Renders the active scene to the active render buffer
 * @param a_Renderer the renderer to use for rendering
 */
void Render(const Handle& a_Renderer);

/**
 * @brief Updates the renderer, cleaning up stuff if needed
 */
void Update(const Handle& a_Renderer);

void SetSettings(
    const Handle& a_Renderer,
    const RendererSettings& a_Settings);
}
