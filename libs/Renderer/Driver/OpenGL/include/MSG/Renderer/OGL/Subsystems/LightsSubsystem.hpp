#pragma once

#include <MSG/OGLCmdBuffer.hpp>
#include <MSG/OGLFence.hpp>
#include <MSG/Renderer/SubsystemInterface.hpp>

#include <Bindings.glsl>
#include <LightsShadowInputs.glsl>
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
struct GaussianBlurSettings;
}

namespace MSG::Renderer {
class Impl;
}

namespace MSG::Renderer {
class LightsVTFSBuffer {
public:
    LightsVTFSBuffer(OGLContext& a_Ctx);
    LightsVTFSBuffer(const LightsVTFSBuffer&) = delete;
    LightsVTFSBuffer(LightsVTFSBuffer&&)      = default;
    std::shared_ptr<OGLTypedBuffer<GLSL::VTFSLightsBuffer>> lightsBuffer; // GLSL::VTFSLightsBuffer
    std::shared_ptr<OGLTypedBufferArray<GLSL::VTFSCluster>> cluster; // GLSL::VTFSCluster * VTFS_CLUSTER_COUNT
    OGLFence executionFence { true };
    OGLCmdBuffer cmdBuffer;
};

constexpr size_t VTFSBufferNbr = 2;
class LightsVTFS {
public:
    explicit LightsVTFS(Renderer::Impl& a_Renderer);

private:
    uint32_t _currentBuffer = 0;
    std::shared_ptr<OGLProgram> _cullingProgram;
    std::array<LightsVTFSBuffer, VTFSBufferNbr> _buffers;

public:
    void Prepare();
    void Update(const SubsystemsLibrary& a_Subsystems);
    LightsVTFSBuffer* buffer;
};

struct LightsIBL {
    LightsIBL(OGLContext& a_Ctx);
    std::shared_ptr<OGLTypedBuffer<GLSL::LightsIBLUBO>> buffer;
    std::array<std::shared_ptr<OGLTextureCube>, SAMPLERS_IBL_COUNT> textures;
};

struct LightsShadows {
    LightsShadows(OGLContext& a_Ctx);
    std::shared_ptr<OGLTypedBuffer<GLSL::ShadowsBase>> dataBuffer;
    std::shared_ptr<OGLTypedBufferArray<GLSL::Camera>> viewportsBuffer;
    std::array<std::shared_ptr<OGLTexture>, SAMPLERS_SHADOW_COUNT> texturesDepth;
};

class LightsSubsystem : public SubsystemInterface {
public:
    LightsSubsystem(Renderer::Impl& a_Renderer);
    void Update(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems) override;
    LightsVTFS vtfs;
    LightsIBL ibls;
    LightsShadows shadows;
    std::shared_ptr<OGLSampler> iblSpecSampler;
    std::shared_ptr<OGLSampler> shadowSampler;
    std::shared_ptr<OGLSampler> shadowSamplerCube;

private:
    template <typename LightType>
    void _PushLight(
        const LightType& a_LightData,
        GLSL::VTFSLightsBuffer&,
        GLSL::LightsIBLUBO&,
        GLSL::ShadowsBase&,
        GLSL::Camera (&a_Viewports)[SHADOW_MAX_VIEWPORTS],
        size_t& a_ViewportIndex,
        const size_t& a_MaxLights);
    void _UpdateShadows(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems);
    OGLFence _executionFence { true };
    OGLCmdBuffer _cmdBuffer;
};
}