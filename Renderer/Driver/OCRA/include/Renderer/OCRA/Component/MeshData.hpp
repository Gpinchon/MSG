#pragma once

#include <Renderer/Handle.hpp>

#include <vector>

TABGRAPH_RENDERER_HANDLE(TabGraph::Renderer);

namespace TabGraph::SG::Component {
struct Mesh;
}

namespace TabGraph::Renderer {
class Primitive;
class Material;
}

namespace TabGraph::Renderer::Component {
struct MeshData {
    MeshData(const Renderer::Handle& a_Renderer, const SG::Component::Mesh& a_Mesh);
    std::vector<std::shared_ptr<Renderer::Primitive>> primitives;
    std::vector<std::shared_ptr<Renderer::Material>>  materials;
};
}