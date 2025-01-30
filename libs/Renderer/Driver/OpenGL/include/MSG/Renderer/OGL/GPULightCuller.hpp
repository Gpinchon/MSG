#pragma once

#include <MSG/Tools/CPUCompute.hpp>

#include <Bindings.glsl>
#include <VTFS.glsl>

#include <array>
#include <vector>

namespace MSG {
class Scene;
class OGLBuffer;
class OGLTextureCubemap;
class OGLProgram;
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
    std::shared_ptr<OGLProgram> _cullingProgram;
    std::array<std::shared_ptr<OGLBuffer>, GPULightCullerBufferNbr> _GPUlightsBuffers;
    std::array<std::shared_ptr<OGLBuffer>, GPULightCullerBufferNbr> _GPUclustersBuffers;
    std::vector<GLSL::VTFSLightsBuffer> _LightsBuffer { GPULightCullerBufferNbr };
    uint32_t _currentLightBuffer = 0;

public:
    std::shared_ptr<OGLBuffer> GPUlightsBuffer;
    std::shared_ptr<OGLBuffer> GPUclusters;
    std::array<std::shared_ptr<OGLTextureCubemap>, VTFS_IBL_MAX> iblSamplers;
};
}
