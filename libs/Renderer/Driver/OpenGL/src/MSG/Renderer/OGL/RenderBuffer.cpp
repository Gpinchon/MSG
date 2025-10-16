#include <MSG/OGLTexture2D.hpp>
#include <MSG/Renderer/Handles.hpp>
#include <MSG/Renderer/OGL/RenderBuffer.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/RenderBuffer.hpp>
#include <MSG/Renderer/Structs.hpp>

Msg::RenderBuffer::Handle Msg::RenderBuffer::Create(
    const Renderer::Handle& a_Renderer,
    const CreateRenderBufferInfo& a_Info)
{
    return std::make_shared<Impl>(a_Renderer->context, a_Info);
}

std::any Msg::RenderBuffer::GetNativeHandle(
    const Handle& a_RenderBuffer)
{
    return (*a_RenderBuffer)->handle;
}

Msg::RenderBuffer::Impl::Impl(
    OGLContext& a_Context,
    const CreateRenderBufferInfo& a_Info)
    : std::shared_ptr<OGLTexture2D>(std::make_shared<OGLTexture2D>(a_Context, OGLTexture2DInfo { .width = a_Info.width, .height = a_Info.height, .levels = 1, .sizedFormat = GL_RGBA8 }))
{
}
