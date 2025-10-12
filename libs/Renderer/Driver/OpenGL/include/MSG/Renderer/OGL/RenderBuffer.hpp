#pragma once

#include <MSG/Renderer/Handles.hpp>

#include <memory>

namespace Msg {
class OGLContext;
class OGLTexture2D;
}

namespace Msg::Renderer {
struct CreateRenderBufferInfo;
}

namespace Msg::Renderer::RenderBuffer {
class Impl : public std::shared_ptr<OGLTexture2D> {
public:
    Impl(
        OGLContext& a_Context,
        const CreateRenderBufferInfo& a_Info);
};
}
