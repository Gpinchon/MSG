#pragma once

#include <Tools/CPUCompute.hpp>

#include <Bindings.glsl>
#include <VTFS.glsl>

#include <array>

namespace TabGraph::Renderer::RAII {
struct Buffer;
struct Program;
struct TextureCubemap;
}

namespace TabGraph::SG {
class Scene;
}

namespace TabGraph::Renderer {
struct Impl;
}

namespace TabGraph::Renderer {
constexpr auto GPULightCullerBufferNbr = 5;
class GPULightCuller {
public:
    explicit GPULightCuller(Renderer::Impl& a_Renderer);
    void operator()(SG::Scene* a_Scene);

private:
    Renderer::Impl& _renderer;
    std::shared_ptr<RAII::Program> _cullingProgram;
    std::array<std::shared_ptr<RAII::Buffer>, GPULightCullerBufferNbr> _GPUlightsBuffers;
    std::array<std::shared_ptr<RAII::Buffer>, GPULightCullerBufferNbr> _GPUclustersBuffers;
    std::array<GLSL::VTFSLightsBuffer*, GPULightCullerBufferNbr> _GPULightsBufferPtrs;
    uint _currentLightBuffer = 0;

public:
    std::shared_ptr<RAII::Buffer> GPUlightsBuffer;
    std::shared_ptr<RAII::Buffer> GPUclusters;
    std::array<std::shared_ptr<RAII::TextureCubemap>, VTFS_IBL_MAX> iblSamplers;
};
}