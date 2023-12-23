#pragma once

#include <Renderer/Handles.hpp>

#include <Renderer/OGL/RAII/Texture.hpp>
#include <Renderer/OGL/RAII/Wrapper.hpp>

namespace TabGraph::Renderer {
struct Context;
struct CreateRenderBufferInfo;
}

namespace TabGraph::Renderer::RenderBuffer {
struct Impl : std::shared_ptr<RAII::Texture2D> {
    Impl(
        Context& a_Context,
        const CreateRenderBufferInfo& a_Info);
};
}
