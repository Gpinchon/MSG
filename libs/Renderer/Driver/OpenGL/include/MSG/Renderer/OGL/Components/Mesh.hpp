#pragma once

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
using Mesh         = std::vector<MeshLod>;
}
