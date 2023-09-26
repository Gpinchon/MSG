#pragma once

#include <Renderer/OGL/RAII/Wrapper.hpp>

#include <Renderer/Handles.hpp>

#include <vector>

namespace TabGraph::Renderer::RAII {
struct Buffer;
}

namespace TabGraph::Renderer {
class Primitive;
}

namespace TabGraph::SG::Component {
class Mesh;
}

namespace TabGraph::Renderer::Component {
struct MeshData {
    MeshData(const Renderer::Handle& a_Renderer, const SG::Component::Mesh& a_Mesh);
    RAII::Wrapper<RAII::Buffer> transformUBO;
    std::vector<std::shared_ptr<Primitive>> primitives;
};
}
