#pragma once

#include <glm/fwd.hpp>

#include <array>
#include <memory>
#include <vector>

namespace TabGraph::Renderer {
class Context;
}

namespace TabGraph::Renderer::RAII {
class Buffer;
}

namespace TabGraph::SG::Component {
class MeshSkin;
}

namespace TabGraph::Renderer::Component {
class MeshSkin {
public:
    MeshSkin(Context& a_Context, const glm::mat4x4& a_Transform, const SG::Component::MeshSkin& a_Skin);
    void Update(Context& a_Context, const glm::mat4x4& a_Transform, const SG::Component::MeshSkin& a_Skin);
    std::shared_ptr<RAII::Buffer> buffer;
    std::shared_ptr<RAII::Buffer> buffer_Previous;

private:
    uint32_t _bufferIndex = 0;
    std::array<std::shared_ptr<RAII::Buffer>, 2> _buffers;
};
}
