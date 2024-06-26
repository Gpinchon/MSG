#pragma once

#include <Tools/CPUCompute.hpp>

#include <Lights.glsl>

#include <array>

namespace TabGraph::SG {
class Scene;
}

namespace TabGraph::Renderer {
class LightCuller {
public:
    LightCuller();
    void operator()(SG::Scene* a_Scene);
    unsigned lightCount = 0;
    std::array<GLSL::LightBase, 1024> lights;
    std::array<GLSL::LightCluster, LIGHT_CLUSTER_COUNT> clusters;

private:
    Tools::CPUCompute<> _compute {};
};
}