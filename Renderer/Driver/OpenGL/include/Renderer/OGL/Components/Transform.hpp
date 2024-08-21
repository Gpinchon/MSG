#pragma once

#include <Renderer/OGL/UniformBuffer.hpp>

#include <Transform.glsl>

#include <glm/glm.hpp>

namespace TabGraph::Renderer::Component {
class Transform : public UniformBufferT<GLSL::TransformUBO> {
public:
    Transform(Context& a_Context, const GLSL::TransformUBO& a_Transform)
        : UniformBufferT(a_Context, a_Transform)
    {
    }
};
}
