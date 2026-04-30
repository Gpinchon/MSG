#pragma once

#include <MSG/OGLTypedBuffer.hpp>

#include <Transform.glsl>

#include <memory>
#include <utility>
#include <vector>

namespace Msg::Renderer {
class Primitive;
}

namespace Msg::Renderer {
using PrimitiveKey = std::pair<std::shared_ptr<Primitive>, size_t>;
using MeshLod      = std::vector<PrimitiveKey>;
class Mesh : public std::vector<MeshLod> {
public:
    Mesh(const Mesh& a_Mesh)
        : vector(a_Mesh)
    {
    }
    Mesh(OGLContext& a_Context, const std::vector<MeshLod>& a_Lods)
        : vector(a_Lods)
    {
    }
};
}
