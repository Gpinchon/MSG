#pragma once

#include <Renderer/OGL/Material.hpp>
#include <Renderer/OGL/RAII/Wrapper.hpp>

#include <Renderer/Handles.hpp>

#include <vector>

namespace TabGraph::Renderer::RAII {
struct Buffer;
}

namespace TabGraph::Renderer {
class Primitive;
}

namespace TabGraph::SG {
class Material;
}

namespace TabGraph::SG::Component {
class Mesh;
}

namespace TabGraph::Renderer::Component {
using PrimitiveKey  = std::pair<std::shared_ptr<Primitive>, std::shared_ptr<Material>>;
using PrimitiveList = std::vector<PrimitiveKey>;
}
