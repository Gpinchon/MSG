#pragma once

#include <MSG/OGLTypedBuffer.hpp>

#include <Transform.glsl>

#include <memory>
#include <utility>
#include <vector>

namespace Msg::Renderer {
class Primitive;
class Material;
}

namespace Msg::Renderer::Component {
using PrimitiveKey = std::pair<std::shared_ptr<Primitive>, std::shared_ptr<Material>>;
using MeshLod      = std::vector<PrimitiveKey>;
class Mesh : public std::vector<MeshLod> {
public:
    Mesh(const Mesh& a_Mesh)
        : vector(a_Mesh)
        , transform(std::make_shared<OGLTypedBuffer<GLSL::TransformUBO>>(*a_Mesh.transform))
    {
    }
    Mesh(OGLContext& a_Context, const Mesh& a_Mesh, const GLSL::TransformUBO& a_Transform = {})
        : vector(a_Mesh)
        , transform(std::make_shared<OGLTypedBuffer<GLSL::TransformUBO>>(a_Context, a_Transform))
    {
    }
    Mesh(OGLContext& a_Context, const GLSL::TransformUBO& a_Transform = {})
        : transform(std::make_shared<OGLTypedBuffer<GLSL::TransformUBO>>(a_Context, a_Transform))
    {
    }
    std::shared_ptr<OGLTypedBuffer<GLSL::TransformUBO>> transform;
};
}
