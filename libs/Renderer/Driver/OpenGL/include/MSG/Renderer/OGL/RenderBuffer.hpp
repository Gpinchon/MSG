#pragma once

#include <MSG/Renderer/Handles.hpp>

#include <MSG/Renderer/OGL/RAII/Texture.hpp>
#include <MSG/Renderer/OGL/RAII/Wrapper.hpp>

namespace MSG {
class OGLContext;
}

namespace MSG::Renderer {
struct CreateRenderBufferInfo;
}

namespace MSG::Renderer::RenderBuffer {
class Impl : public std::shared_ptr<RAII::Texture2D> {
public:
    Impl(
        OGLContext& a_Context,
        const CreateRenderBufferInfo& a_Info);
};
}
