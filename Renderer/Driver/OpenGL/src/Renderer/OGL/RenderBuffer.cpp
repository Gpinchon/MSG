#include <Renderer/Handles.hpp>
#include <Renderer/Structs.hpp>

#include <Renderer/OGL/RenderBuffer.hpp>
#include <Renderer/OGL/Renderer.hpp>

namespace TabGraph::Renderer::RenderBuffer {
Handle Create(
    const Renderer::Handle& a_Renderer,
    const CreateRenderBufferInfo& a_Info)
{
    return std::make_shared<Impl>(a_Renderer->context, a_Info);
}

Impl::Impl(
    Context& a_Context,
    const CreateRenderBufferInfo& a_Info)
    : std::shared_ptr<RAII::Texture2D>(RAII::MakePtr<RAII::Texture2D>(a_Context, a_Info.width, a_Info.height, 1, GL_RGBA8))
{
}
}
