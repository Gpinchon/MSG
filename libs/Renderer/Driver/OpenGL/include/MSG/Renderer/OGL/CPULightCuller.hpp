#pragma once

#include <MSG/Tools/CPUCompute.hpp>

#include <VTFS.glsl>

#include <array>

namespace MSG {
class Scene;
class OGLContext;
class OGLBuffer;
class OGLTextureCubemap;
}

namespace MSG::Renderer {
class Impl;
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
    std::shared_ptr<OGLBuffer> GPUlightsBuffer;
    std::shared_ptr<OGLBuffer> GPUclusters;
    std::array<std::shared_ptr<OGLTextureCubemap>, VTFS_IBL_MAX> iblSamplers;
};
}
