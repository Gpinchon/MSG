#pragma once

#include <MSG/OGLCmdBuffer.hpp>
#include <MSG/OGLFence.hpp>
#include <MSG/Renderer/OGL/Subsystems/SubsystemInterface.hpp>

#include <MSG/Renderer/OGL/LightCullerVTFS.hpp>

#include <Bindings.glsl>

namespace MSG {
class OGLContext;
class OGLSampler;
template <typename>
class OGLTypedBuffer;
}

namespace MSG::Renderer::GLSL {
struct LightsIBLUBO;
struct ShadowsBase;
}

namespace MSG::Renderer {
class Impl;
}

namespace MSG::Renderer {
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
    LightCullerVTFS vtfs;
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