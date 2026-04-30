#pragma once

#include <memory>

namespace Msg {
class OGLContext;
template <typename>
class OGLTypedBufferArray;
}

namespace Msg::Renderer::GLSL {
struct TransformUBO;
}

namespace Msg::Renderer {
class MeshInstances {
public:
    MeshInstances()                = default;
    MeshInstances(MeshInstances&&) = default;
    MeshInstances(OGLContext& a_Ctx, const uint32_t& a_Instances);
    MeshInstances& operator=(MeshInstances&& a_Rhs)
    {
        instances       = std::move(a_Rhs.instances);
        transformBuffer = std::move(a_Rhs.transformBuffer);
        a_Rhs.instances = 0;
        return *this;
    }
    uint32_t instances = 0;
    std::shared_ptr<OGLTypedBufferArray<GLSL::TransformUBO>> transformBuffer;
};
}