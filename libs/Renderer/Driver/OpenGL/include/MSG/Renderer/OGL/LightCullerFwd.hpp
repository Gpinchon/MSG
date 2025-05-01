#pragma once

#include <MSG/OGLTypedBuffer.hpp>
#include <MSG/Renderer/OGL/LightCullerVTFS.hpp>

#include <Bindings.glsl>
#include <LightsIBLInputs.glsl>
#include <LightsShadowInputs.glsl>

#include <array>
#include <vector>

namespace MSG {
class Scene;
class OGLContext;
class OGLTextureCube;
class OGLTexture;
class OGLProgram;
}

namespace MSG::Renderer::Component {
class LightData;
}

namespace MSG::Renderer {
class Impl;
}

namespace MSG::Renderer {
struct LightCullerFwdIBL {
    LightCullerFwdIBL(OGLContext& a_Ctx)
        : buffer(std::make_shared<OGLTypedBuffer<GLSL::LightsIBLUBO>>(a_Ctx))
    {
    }
    std::shared_ptr<OGLTypedBuffer<GLSL::LightsIBLUBO>> buffer;
    std::array<std::shared_ptr<OGLTextureCube>, SAMPLERS_IBL_COUNT> textures;
};

struct LightCullerFwdShadows {
    LightCullerFwdShadows(OGLContext& a_Ctx)
        : buffer(std::make_shared<OGLTypedBuffer<GLSL::ShadowsBase>>(a_Ctx))
    {
    }
    std::shared_ptr<OGLTypedBuffer<GLSL::ShadowsBase>> buffer;
    std::array<std::shared_ptr<OGLTexture>, SAMPLERS_SHADOW_COUNT> texturesDepth;
    std::array<std::shared_ptr<OGLTexture>, SAMPLERS_SHADOW_COUNT> texturesMoments;
};

class LightCullerFwd {
public:
    explicit LightCullerFwd(Renderer::Impl& a_Renderer);
    void operator()(Scene* a_Scene, const std::shared_ptr<OGLBuffer>& a_CameraUBO);

private:
    template <typename LightType>
    void _PushLight(const LightType&, GLSL::LightsIBLUBO&, GLSL::ShadowsBase&, const size_t&);
    Renderer::Impl& _renderer;

public:
    LightCullerVTFS vtfs;
    LightCullerFwdIBL ibls;
    LightCullerFwdShadows shadows;
};
}
