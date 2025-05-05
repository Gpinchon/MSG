#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/RendererPath.hpp>

MSG::Renderer::Path::Path(Renderer::Impl& a_Renderer)
    : cmdBuffer(a_Renderer.context)
{
}