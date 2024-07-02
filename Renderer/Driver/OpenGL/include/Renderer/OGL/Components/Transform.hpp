#pragma once

#include <Renderer/OGL/UniformBuffer.hpp>

#include <Transform.glsl>

#include <glm/glm.hpp>

namespace TabGraph::Renderer::Component {
struct Transform : UniformBufferT<GLSL::Transform> {
    Transform(Context& a_Context, const GLSL::Transform& a_Transform)
        : UniformBufferT(a_Context, a_Transform)
    {
    }
};
}
