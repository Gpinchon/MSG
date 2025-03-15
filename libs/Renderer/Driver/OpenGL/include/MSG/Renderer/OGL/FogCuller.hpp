#pragma once

#include <MSG/Image.hpp>

#include <memory>

namespace MSG::Renderer {
class Impl;
class LightCullerVTFSBuffer;
}

namespace MSG {
class OGLTexture3D;
class OGLContext;
class OGLProgram;
class OGLBuffer;
class Scene;
};

namespace MSG::Renderer {
class FogCuller {
public:
    FogCuller(Renderer::Impl& a_Renderer);
    void Update(
        const Scene& a_Scene,
        const LightCullerVTFSBuffer& a_VTFSBuffer,
        const std::shared_ptr<OGLBuffer>& a_CameraBuffer,
        const std::shared_ptr<OGLBuffer>& a_FrameInfoBuffer);
    OGLContext& context;
    Image image;
    std::shared_ptr<OGLTexture3D> texture;
    std::shared_ptr<OGLProgram> cullingProgram;
};
}