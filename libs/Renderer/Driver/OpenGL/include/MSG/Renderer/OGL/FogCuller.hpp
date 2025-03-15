#pragma once

#include <MSG/Image.hpp>

#include <memory>

namespace MSG::Renderer {
class Impl;
}

namespace MSG {
class OGLTexture3D;
class OGLContext;
class OGLProgram;
class Scene;
};

namespace MSG::Renderer {
class FogCuller {
public:
    FogCuller(Renderer::Impl& a_Renderer);
    void Update(const Scene& a_Scene);
    OGLContext& context;
    Image image;
    std::shared_ptr<OGLTexture3D> texture;
    std::shared_ptr<OGLProgram> cullingProgram;
};
}