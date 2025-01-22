#pragma once

#include <OCRA/Handles.hpp>

#include <memory>
#include <vector>

namespace MSG::SG::Component {
struct Mesh;
}

namespace MSG::Renderer {
struct Primitive;
struct Material;
struct Impl;
}

namespace MSG::Renderer::Component {
struct MeshData {
    MeshData(
        Renderer::Impl* a_Renderer,
        const Mesh& a_Mesh);
    std::vector<std::shared_ptr<Renderer::Primitive>> primitives;
    std::vector<std::shared_ptr<Renderer::Material>> materials;
    std::vector<OCRA::Pipeline::Handle> graphicsPipelines;
};
}
