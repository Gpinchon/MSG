#pragma once

#include <MSG/Tools/CPUCompute.hpp>

#include <VTFS.glsl>

#include <array>

namespace MSG {
class Scene;
class OGLContext;
}

namespace MSG::Renderer {
class Impl;
}

namespace MSG::Renderer::RAII {
class Buffer;
class TextureCubemap;
}

namespace MSG::Renderer {
class CPULightCuller {
public:
    explicit CPULightCuller(Renderer::Impl& a_Renderer);
    void operator()(Scene* a_Scene);

private:
    OGLContext& _context;
    GLSL::VTFSLightsBuffer _lights = {};
    std::array<GLSL::VTFSCluster, VTFS_CLUSTER_COUNT> _clusters;
    Tools::CPUCompute<VTFS_LOCAL_SIZE, 1, 1> _compute {};

public:
    std::shared_ptr<RAII::Buffer> GPUlightsBuffer;
    std::shared_ptr<RAII::Buffer> GPUclusters;
    std::array<std::shared_ptr<RAII::TextureCubemap>, VTFS_IBL_MAX> iblSamplers;
};
}
