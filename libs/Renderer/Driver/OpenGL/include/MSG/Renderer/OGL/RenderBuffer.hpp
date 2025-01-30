#pragma once

#include <MSG/Renderer/Handles.hpp>

#include <memory>

namespace MSG {
class OGLContext;
class OGLTexture2D;
}

namespace MSG::Renderer {
struct CreateRenderBufferInfo;
}

namespace MSG::Renderer::RenderBuffer {
class Impl : public std::shared_ptr<OGLTexture2D> {
public:
    Impl(
        OGLContext& a_Context,
        const CreateRenderBufferInfo& a_Info);
};
}
