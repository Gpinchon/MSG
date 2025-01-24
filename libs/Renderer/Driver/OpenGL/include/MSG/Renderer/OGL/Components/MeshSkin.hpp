#pragma once

#include <glm/fwd.hpp>

#include <array>
#include <memory>
#include <vector>

namespace MSG {
class OGLContext;
}

namespace MSG::Renderer::RAII {
class Buffer;
}

namespace MSG {
class MeshSkin;
}

namespace MSG::Renderer::Component {
class MeshSkin {
public:
    MeshSkin(OGLContext& a_Context, const glm::mat4x4& a_Transform, const MSG::MeshSkin& a_Skin);
    void Update(OGLContext& a_Context, const glm::mat4x4& a_Transform, const MSG::MeshSkin& a_Skin);
    std::shared_ptr<RAII::Buffer> buffer;
    std::shared_ptr<RAII::Buffer> buffer_Previous;

private:
    uint32_t _bufferIndex = 0;
    std::array<std::shared_ptr<RAII::Buffer>, 2> _buffers;
};
}
