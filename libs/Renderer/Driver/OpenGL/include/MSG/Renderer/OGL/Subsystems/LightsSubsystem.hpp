#pragma once

#include <MSG/OGLCmdBuffer.hpp>
#include <MSG/OGLFence.hpp>
#include <MSG/Renderer/OGL/Subsystems/SubsystemInterface.hpp>

#include <Bindings.glsl>
#include <LightsVTFS.glsl>

namespace MSG {
class OGLContext;
class OGLSampler;
class OGLTextureCube;
template <typename>
class OGLTypedBuffer;
template <typename>
class OGLTypedBufferArray;
}

namespace MSG::Renderer::GLSL {
struct LightsIBLUBO;
struct ShadowsBase;
}

namespace MSG::Renderer {
class Impl;
}

namespace MSG::Renderer {
class LightsVTFSBuffer {
public:
    LightsVTFSBuffer(OGLContext& a_Ctx);
    std::shared_ptr<OGLTypedBuffer<GLSL::VTFSLightsBuffer>> lightsBuffer; // GLSL::VTFSLightsBuffer
    std::shared_ptr<OGLTypedBufferArray<GLSL::VTFSCluster>> cluster; // GLSL::VTFSCluster * VTFS_CLUSTER_COUNT
    GLSL::VTFSLightsBuffer lightsBufferCPU;
    OGLFence executionFence { true };
    OGLCmdBuffer cmdBuffer;
};

constexpr size_t VTFSBufferNbr = 2;
class LightsVTFS {
public:
    explicit LightsVTFS(Renderer::Impl& a_Renderer);

private:
    OGLContext& _context;
    uint32_t _currentBuffer = 0;
    std::shared_ptr<OGLProgram> _cullingProgram;
    std::array<LightsVTFSBuffer, VTFSBufferNbr> _buffers { LightsVTFSBuffer(_context), LightsVTFSBuffer(_context) };

public:
    void Prepare();
    template <typename LightType>
    bool PushLight(const LightType&);
    void Cull(const std::shared_ptr<OGLBuffer>& a_CameraUBO);
    LightsVTFSBuffer* buffer;
};

struct LightsIBL {
    LightsIBL(OGLContext& a_Ctx);
    std::shared_ptr<OGLTypedBuffer<GLSL::LightsIBLUBO>> buffer;
    std::array<std::shared_ptr<OGLTextureCube>, SAMPLERS_IBL_COUNT> textures;
};

struct LightsShadows {
    LightsShadows(OGLContext& a_Ctx);
    std::shared_ptr<OGLTypedBuffer<GLSL::ShadowsBase>> buffer;
    std::array<std::shared_ptr<OGLTexture>, SAMPLERS_SHADOW_COUNT> texturesDepth;
    std::array<std::shared_ptr<OGLTexture>, SAMPLERS_SHADOW_COUNT> texturesMoments;
};

class LightsSubsystem : public SubsystemInterface {
public:
    LightsSubsystem(Renderer::Impl& a_Renderer);
    void Update(Renderer::Impl& a_Renderer, const SubsystemLibrary& a_Subsystems) override;
    LightsVTFS vtfs;
    LightsIBL ibls;
    LightsShadows shadows;
    std::shared_ptr<OGLSampler> iblSpecSampler;
    std::shared_ptr<OGLSampler> shadowSampler;

private:
    template <typename LightType>
    void _PushLight(const LightType& a_LightData, GLSL::LightsIBLUBO&, GLSL::ShadowsBase&, const size_t&);
    void _UpdateLights(Renderer::Impl& a_Renderer);
    void _UpdateShadows(Renderer::Impl& a_Renderer, const SubsystemLibrary& a_Subsystems);
    OGLFence _executionFence { true };
    OGLCmdBuffer _cmdBuffer;
};
}