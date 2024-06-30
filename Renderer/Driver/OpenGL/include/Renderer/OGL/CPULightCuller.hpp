#pragma once

#include <Renderer/OGL/RAII/Buffer.hpp>

#include <Tools/CPUCompute.hpp>

#include <Lights.glsl>

#include <array>

namespace TabGraph::SG {
class Scene;
}

namespace TabGraph::Renderer {
struct Context;
}

namespace TabGraph::Renderer {
class CPULightCuller {
public:
    explicit CPULightCuller(Context& a_Context);
    void operator()(SG::Scene* a_Scene);

private:
    Context& _context;
    GLSL::LightsBuffer _lights;
    GLSL::LightCluster _clusters[LIGHT_CLUSTER_COUNT];
    Tools::CPUCompute<> _compute {};

public:
    std::shared_ptr<RAII::Buffer> GPUlightsBuffer;
    std::shared_ptr<RAII::Buffer> GPUclusters;
};
}