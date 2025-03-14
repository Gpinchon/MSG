#pragma once

#include <MSG/Image.hpp>

#include <memory>

namespace MSG {
class OGLTexture3D;
class OGLContext;
class Scene;
};

namespace MSG::Renderer {
class VolumetricFogCuller {
    VolumetricFogCuller(OGLContext& a_Ctx);
    void Update(const Scene& a_Scene);
    Image image;
    std::shared_ptr<OGLTexture3D> texture;
};
}