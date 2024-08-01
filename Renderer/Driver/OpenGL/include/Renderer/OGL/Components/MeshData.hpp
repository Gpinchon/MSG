#pragma once

#include <memory>
#include <utility>
#include <vector>

namespace TabGraph::Renderer {
class Primitive;
class Material;
}

namespace TabGraph::Renderer::Component {
using PrimitiveKey  = std::pair<std::shared_ptr<Primitive>, std::shared_ptr<Material>>;
using PrimitiveList = std::vector<PrimitiveKey>;
}
