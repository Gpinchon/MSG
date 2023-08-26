#include <Renderer/OGL/Renderer.hpp>
#include <Renderer/Structs.hpp>
#include <SG/Scene/Scene.hpp>

#include <Renderer/OGL/RAII/DebugGroup.hpp>
#include <Renderer/OGL/RenderBuffer.hpp>

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
    auto& renderer = *a_Renderer;
    renderer.context.PushRenderCmd(
        [&renderer, &a_Buffer] {
            RAII::DebugGroup("Render Scene");
            float color[4] = { 1, 0, 0, 1 };
            glClearTexImage(
                **a_Buffer,
                0, // level
                GL_RGBA,
                GL_FLOAT,
                color);
            /*glNamedFramebufferTexture(
                *renderer.mainFrameBuffer,
                GL_COLOR_ATTACHMENT0,
                **a_Buffer, 0);
            glClearFra*/
        },
        false);
}

void Update(const Handle& a_Renderer)
{
}
}
