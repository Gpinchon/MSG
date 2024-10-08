#pragma once

#include <Tools/CPUCompute.hpp>

#include <VTFS.glsl>

#include <array>

namespace TabGraph::SG {
class Scene;
}

namespace TabGraph::Renderer {
class Context;
class Impl;
}

namespace TabGraph::Renderer::RAII {
class Buffer;
class TextureCubemap;
}

namespace TabGraph::Renderer {
class CPULightCuller {
public:
    explicit CPULightCuller(Renderer::Impl& a_Renderer);
    void operator()(SG::Scene* a_Scene);

private:
    Context& _context;
    GLSL::VTFSLightsBuffer _lights = {};
    std::array<GLSL::VTFSCluster, VTFS_CLUSTER_COUNT> _clusters;
    Tools::CPUCompute<VTFS_LOCAL_SIZE, 1, 1> _compute {};

public:
    std::shared_ptr<RAII::Buffer> GPUlightsBuffer;
    std::shared_ptr<RAII::Buffer> GPUclusters;
    std::array<std::shared_ptr<RAII::TextureCubemap>, VTFS_IBL_MAX> iblSamplers;
};
}
