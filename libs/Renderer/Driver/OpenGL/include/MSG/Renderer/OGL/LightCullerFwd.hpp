#pragma once

#include <MSG/Renderer/OGL/LightCullerVTFS.hpp>
#include <MSG/Renderer/OGL/UniformBuffer.hpp>

#include <Bindings.glsl>
#include <FwdLights.glsl>

#include <array>
#include <vector>

namespace MSG {
class Scene;
class OGLContext;
class OGLTextureCubemap;
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
        : UBO(a_Ctx)
    {
    }
    UniformBufferT<GLSL::FwdIBL> UBO;
    std::array<std::shared_ptr<OGLTextureCubemap>, FWD_LIGHT_MAX_IBL> textures;
};

struct LightCullerFwdShadows {
    LightCullerFwdShadows(OGLContext& a_Ctx)
        : UBO(a_Ctx)
    {
    }
    UniformBufferT<GLSL::FwdShadowsBase> UBO;
    std::array<std::shared_ptr<OGLTexture>, FWD_LIGHT_MAX_SHADOWS> textures;
};

class LightCullerFwd {
public:
    explicit LightCullerFwd(Renderer::Impl& a_Renderer);
    void operator()(Scene* a_Scene, const std::shared_ptr<OGLBuffer>& a_CameraUBO);

private:
    template <typename LightType>
    void _PushLight(const LightType&, GLSL::FwdIBL&, GLSL::FwdShadowsBase&);
    Renderer::Impl& _renderer;

public:
    LightCullerVTFS vtfs;
    LightCullerFwdIBL ibl;
    LightCullerFwdShadows shadows;
};
}
