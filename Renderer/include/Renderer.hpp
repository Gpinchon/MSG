#pragma once

#include <ECS/Registry.hpp>

#include <OCRA/Instance.hpp>

#include <OCRA/Image/Image.hpp>

#include <memory>

namespace TabGraph::Renderer {
struct Impl;
typedef std::shared_ptr<Impl> Handle;
}

namespace TabGraph::Renderer {
/** @return an instance of Renderer */
Handle Create(const OCRA::Application::Info& a_Info);

/**
* @brief loads the necessary data for rendering
*/
void Load(
    const Handle& a_Renderer,
    const std::shared_ptr<ECS::DefaultRegistry>& a_Registry);

/**
* @brief loads the necessary data for rendering
*/
void Load(
    const Handle& a_Renderer,
    const std::shared_ptr<ECS::DefaultRegistry>& a_Registry,
    const ECS::DefaultRegistry::EntityIDType& a_Begin,
    const ECS::DefaultRegistry::EntityIDType& a_End);

/**
* @brief unloads the rendering data
*/
void Unload(const Handle& a_Renderer, const std::shared_ptr<ECS::DefaultRegistry>& a_Registry);

/**
* @brief unloads the rendering data
*/
void Unload(
    const Handle& a_Renderer,
    const std::shared_ptr<ECS::DefaultRegistry>& a_Registry,
    const ECS::DefaultRegistry::EntityIDType& a_Begin,
    const ECS::DefaultRegistry::EntityIDType& a_End);

/**
* @arg a_Renderer : the renderer to use for rendering
* @arg a_Registry : the ECS registry to register
* @arg a_Image    : the image to render to
*/
void Render(
    const Handle& a_Renderer,
    const std::shared_ptr<ECS::DefaultRegistry>& a_Registry,
    const OCRA::Image::Handle& a_Image);
}