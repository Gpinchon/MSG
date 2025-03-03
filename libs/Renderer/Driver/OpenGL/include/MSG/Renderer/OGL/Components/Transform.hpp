#pragma once

#include <MSG/OGLTypedBuffer.hpp>

#include <Transform.glsl>

#include <glm/glm.hpp>

namespace MSG::Renderer::Component {
class Transform {
public:
    Transform(OGLContext& a_Context, const GLSL::TransformUBO& a_Transform)
        : buffer(std::make_shared<OGLTypedBuffer<GLSL::TransformUBO>>(a_Context, a_Transform))
    {
    }
    std::shared_ptr<OGLTypedBuffer<GLSL::TransformUBO>> buffer;
};
}
