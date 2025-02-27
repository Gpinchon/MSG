#pragma once

#include <MSG/Tools/MakeArrayHelper.hpp>

#include <Bindings.glsl>
#include <VTFS.glsl>

#include <array>

namespace MSG {
class Scene;
class OGLContext;
class OGLBuffer;
class OGLProgram;
}

namespace MSG::Renderer::Component {
class LightData;
}

namespace MSG::Renderer {
class Impl;
}

namespace MSG::Renderer {
class LightCullerVTFSBuffer {
public:
    LightCullerVTFSBuffer(OGLContext& a_Ctx);
    void clear() { lightsBufferCPU.count = 0; }
    std::shared_ptr<OGLBuffer> lightsBuffer; // GLSL::VTFSLightsBuffer
    std::shared_ptr<OGLBuffer> cluster; // GLSL::VTFSCluster * VTFS_CLUSTER_COUNT
    GLSL::VTFSLightsBuffer lightsBufferCPU;
};

constexpr size_t VTFSBufferNbr = 2;
class LightCullerVTFS {
public:
    explicit LightCullerVTFS(Renderer::Impl& a_Renderer);
    void operator()(Scene* a_Scene, const std::shared_ptr<OGLBuffer>& a_CameraUBO);

private:
    OGLContext& _context;
    uint32_t _currentBuffer = 0;
    std::shared_ptr<OGLProgram> _vtfsCullingProgram;
    std::array<LightCullerVTFSBuffer, VTFSBufferNbr> _buffers = Tools::MakeArray<LightCullerVTFSBuffer, VTFSBufferNbr>(_context);

public:
    void Prepare();
    template <typename LightType>
    bool PushLight(const LightType&);
    void Cull(const std::shared_ptr<OGLBuffer>& a_CameraUBO);
    LightCullerVTFSBuffer& buffer;
};
}
