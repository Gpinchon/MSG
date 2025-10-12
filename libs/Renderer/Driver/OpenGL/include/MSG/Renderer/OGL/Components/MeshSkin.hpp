#pragma once

#include <glm/fwd.hpp>

#include <array>
#include <memory>
#include <vector>

namespace Msg {
class OGLContext;
class OGLBuffer;
}

namespace Msg {
class MeshSkin;
}

namespace Msg::Renderer::Component {
class MeshSkin {
public:
    MeshSkin(OGLContext& a_Context, const glm::mat4x4& a_Transform, const Msg::MeshSkin& a_Skin);
    void Update(OGLContext& a_Context, const glm::mat4x4& a_Transform, const Msg::MeshSkin& a_Skin);
    std::shared_ptr<OGLBuffer> buffer;
    std::shared_ptr<OGLBuffer> buffer_Previous;

private:
    uint32_t _bufferIndex = 0;
    std::array<std::shared_ptr<OGLBuffer>, 2> _buffers;
};
}
