#pragma once

#include <memory>
#include <vector>

namespace TabGraph::SG::Component {
struct Mesh;
}

namespace TabGraph::Renderer {
struct Primitive;
struct Material;
struct Impl;
}

namespace TabGraph::Renderer::Component {
struct MeshData {
    MeshData(Renderer::Impl* a_Renderer, const SG::Component::Mesh& a_Mesh);
    std::vector<std::shared_ptr<Renderer::Primitive>> primitives;
    std::vector<std::shared_ptr<Renderer::Material>>  materials;
};
}