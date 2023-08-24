#include <Renderer/Handles.hpp>
#include <Renderer/Structs.hpp>

#include <Renderer/OGL/RenderBuffer.hpp>
#include <Renderer/OGL/Renderer.hpp>

namespace TabGraph::Renderer::RenderBuffer {
Handle Create(
    const Renderer::Handle& a_Renderer,
    const CreateRenderBufferInfo& a_Info)
{
    return std::make_shared<Impl>(a_Renderer, a_Info);
}

Impl::Impl(
    const Renderer::Handle& a_Renderer,
    const CreateRenderBufferInfo& a_Info)
    : renderer(a_Renderer)
{
    a_Renderer->PushResourceCreationCmd(
        [this, &a_Info] {
            glCreateTextures(GL_TEXTURE_2D, 1, &handle);
            glTextureStorage2D(handle, 1, GL_RGB8, a_Info.width, a_Info.height);
        },
        true);
}

Impl::~Impl()
{
    renderer.lock()->PushResourceDestructionCmd([handle = handle] { glDeleteTextures(1, &handle); }, false);
}
}
