#pragma once

#include <glm/mat4x4.hpp>

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
struct MeshSkin;
struct Transform;
}

namespace TabGraph::Renderer::Component {
class MeshSkin {
public:
    MeshSkin(Context& a_Context, const glm::mat4x4& a_Transform, const SG::Component::MeshSkin& a_Skin);
    void Update(Context& a_Context, const glm::mat4x4& a_Transform, const SG::Component::MeshSkin& a_Skin);
    const uint32_t skinSize;
    uint32_t bufferIndex          = 0;
    uint32_t bufferIndex_Previous = 0;
    std::array<std::shared_ptr<RAII::Buffer>, 2> buffers;
};
}
