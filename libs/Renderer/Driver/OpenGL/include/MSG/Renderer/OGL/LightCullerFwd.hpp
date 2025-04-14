#pragma once

#include <MSG/OGLTypedBuffer.hpp>
#include <MSG/Renderer/OGL/LightCullerVTFS.hpp>

#include <Bindings.glsl>
#include <FwdLights.glsl>

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
        : buffer(std::make_shared<OGLTypedBuffer<GLSL::FwdIBL>>(a_Ctx))
    {
    }
    std::shared_ptr<OGLTypedBuffer<GLSL::FwdIBL>> buffer;
    std::array<std::shared_ptr<OGLTextureCube>, FWD_LIGHT_MAX_IBL> textures;
};

struct LightCullerFwdShadows {
    LightCullerFwdShadows(OGLContext& a_Ctx)
        : buffer(std::make_shared<OGLTypedBuffer<GLSL::FwdShadowsBase>>(a_Ctx))
    {
    }
    std::shared_ptr<OGLTypedBuffer<GLSL::FwdShadowsBase>> buffer;
    std::array<std::shared_ptr<OGLTexture>, FWD_LIGHT_MAX_SHADOWS> textures;
};

class LightCullerFwd {
public:
    explicit LightCullerFwd(Renderer::Impl& a_Renderer);
    void operator()(Scene* a_Scene, const std::shared_ptr<OGLBuffer>& a_CameraUBO);

private:
    template <typename LightType>
    void _PushLight(const LightType&, GLSL::FwdIBL&, GLSL::FwdShadowsBase&, const size_t&);
    Renderer::Impl& _renderer;

public:
    LightCullerVTFS vtfs;
    LightCullerFwdIBL ibls;
    LightCullerFwdShadows shadows;
};
}
