#pragma once

#include <Renderer/Handles.hpp>

#include <Renderer/OGL/RAII/Texture.hpp>
#include <Renderer/OGL/RAII/Wrapper.hpp>

namespace TabGraph::Renderer {
class Context;
struct CreateRenderBufferInfo;
}

namespace TabGraph::Renderer::RenderBuffer {
class Impl : public std::shared_ptr<RAII::Texture2D> {
public:
    Impl(
        Context& a_Context,
        const CreateRenderBufferInfo& a_Info);
};
}
