#pragma once

#include <Renderer/OGL/UniformBuffer.hpp>

#include <glm/glm.hpp>

namespace TabGraph::Renderer::Component {
struct Transform : UniformBufferT<glm::mat4> {
    Transform(Context& a_Context, const glm::mat4& a_Transform)
        : UniformBufferT(a_Context, a_Transform)
    {
    }
};
}
