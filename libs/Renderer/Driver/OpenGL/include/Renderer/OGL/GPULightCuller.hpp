#pragma once

#include <Tools/CPUCompute.hpp>

#include <Bindings.glsl>
#include <VTFS.glsl>

#include <array>
#include <vector>

namespace MSG::Renderer::RAII {
class Buffer;
class Program;
class TextureCubemap;
}

namespace MSG {
class Scene;
}

namespace MSG::Renderer {
class Impl;
}

namespace MSG::Renderer {
constexpr auto GPULightCullerBufferNbr = 2;
class GPULightCuller {
public:
    explicit GPULightCuller(Renderer::Impl& a_Renderer);
    void operator()(Scene* a_Scene);

private:
    Renderer::Impl& _renderer;
    std::shared_ptr<RAII::Program> _cullingProgram;
    std::array<std::shared_ptr<RAII::Buffer>, GPULightCullerBufferNbr> _GPUlightsBuffers;
    std::array<std::shared_ptr<RAII::Buffer>, GPULightCullerBufferNbr> _GPUclustersBuffers;
    std::vector<GLSL::VTFSLightsBuffer> _LightsBuffer { GPULightCullerBufferNbr };
    uint32_t _currentLightBuffer = 0;

public:
    std::shared_ptr<RAII::Buffer> GPUlightsBuffer;
    std::shared_ptr<RAII::Buffer> GPUclusters;
    std::array<std::shared_ptr<RAII::TextureCubemap>, VTFS_IBL_MAX> iblSamplers;
};
}
