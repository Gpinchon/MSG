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
}

namespace TabGraph::Renderer {
class CPULightCuller {
public:
    explicit CPULightCuller(Renderer::Impl& a_Renderer);
    void operator()(SG::Scene* a_Scene);

private:
    Context& _context;
    GLSL::VTFSLightsBuffer _lights;
    GLSL::VTFSCluster _clusters[VTFS_CLUSTER_COUNT];
    Tools::CPUCompute<VTFS_LOCAL_SIZE, 1, 1> _compute {};

public:
    std::shared_ptr<RAII::Buffer> GPUlightsBuffer;
    std::shared_ptr<RAII::Buffer> GPUclusters;
};
}