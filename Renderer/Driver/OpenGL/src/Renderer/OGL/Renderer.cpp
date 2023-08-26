#include <Renderer/OGL/Renderer.hpp>
#include <Renderer/Structs.hpp>
#include <SG/Scene/Scene.hpp>

#ifdef _WIN32
#include <Renderer/OGL/Win32/Context.hpp>
#include <Renderer/OGL/Win32/Error.hpp>
#include <Renderer/OGL/Win32/Window.hpp>
#endif

#include <GL/wglew.h>

#include <stdexcept>

namespace TabGraph::Renderer {
Handle Create(const CreateRendererInfo& a_Info)
{
    return Handle(new Impl(a_Info));
}

TabGraph::Renderer::Impl::Impl(const CreateRendererInfo& a_Info)
    : name(a_Info.name)
    , version(a_Info.applicationVersion)
{
}

void Load(
    const Handle& a_Renderer,
    const SG::Scene& a_Scene)
{
}

void Load(
    const Handle& a_Renderer,
    const uint32_t a_EntityID)
{
}

void Unload(
    const Handle& a_Renderer,
    const SG::Scene& a_Scene)
{
}

void Unload(
    const Handle& a_Renderer,
    const uint32_t a_EntityID)
{
}

void Render(
    const Handle& a_Renderer,
    const SG::Scene& a_Scene,
    const RenderBuffer::Handle& a_Buffer)
{
}

void Update(const Handle& a_Renderer)
{
}
}
