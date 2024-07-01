#pragma once

#include <Tools/CPUCompute.hpp>

#include <VTFS.glsl>

#include <array>

namespace TabGraph::Renderer::RAII {
struct Buffer;
struct Program;
}

namespace TabGraph::SG {
class Scene;
}

namespace TabGraph::Renderer {
struct Impl;
}

namespace TabGraph::Renderer {
class GPULightCuller {
public:
    explicit GPULightCuller(Renderer::Impl& a_Renderer);
    void operator()(SG::Scene* a_Scene);

private:
    Renderer::Impl& _renderer;
    GLSL::VTFSLightsBuffer _lights;
    std::shared_ptr<RAII::Program> _cullingProgram;
    std::array<std::shared_ptr<RAII::Buffer>, 3> _GPUlightsBuffers;
    uint _currentLightBuffer = 0;

public:
    std::shared_ptr<RAII::Buffer> GPUlightsBuffer;
    std::shared_ptr<RAII::Buffer> GPUclusters;
};
}