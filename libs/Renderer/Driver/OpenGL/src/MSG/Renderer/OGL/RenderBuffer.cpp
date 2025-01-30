#include <MSG/OGLTexture2D.hpp>
#include <MSG/Renderer/Handles.hpp>
#include <MSG/Renderer/OGL/RenderBuffer.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/Structs.hpp>

namespace MSG::Renderer::RenderBuffer {
Handle Create(
    const Renderer::Handle& a_Renderer,
    const CreateRenderBufferInfo& a_Info)
{
    return std::make_shared<Impl>(a_Renderer->context, a_Info);
}

Impl::Impl(
    OGLContext& a_Context,
    const CreateRenderBufferInfo& a_Info)
    : std::shared_ptr<OGLTexture2D>(std::make_shared<OGLTexture2D>(a_Context, a_Info.width, a_Info.height, 1, GL_RGBA8))
{
}
}
