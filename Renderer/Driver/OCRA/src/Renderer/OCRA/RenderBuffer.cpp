#include <Renderer/Renderer.hpp>

#include <Renderer/OCRA/Renderer.hpp>
#include <Renderer/OCRA/RenderBuffer.hpp>

#include <OCRA/OCRA.hpp>

namespace TabGraph::Renderer::RenderBuffer {
Handle Create(const Renderer::Handle& a_Renderer, const CreateRenderBufferInfo& a_Info)
{
    return std::make_shared<Impl>(a_Renderer, a_Info);
}
}