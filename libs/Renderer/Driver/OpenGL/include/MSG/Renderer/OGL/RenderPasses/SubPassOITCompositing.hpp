#pragma once

#include <MSG/Renderer/RenderSubPassInterface.hpp>

#include <memory>

namespace MSG {
class OGLTexture3D;
class OGLProgram;
}

namespace MSG::Renderer {
class SubPassOITCompositing : public RenderSubPassInterface {
public:
    SubPassOITCompositing(Renderer::Impl& a_Renderer);
    void Update(Renderer::Impl& a_Renderer, RenderPassInterface* a_ParentPass) override;
    void Render(Impl& a_Renderer) override;
    std::shared_ptr<OGLProgram> shader;
    std::shared_ptr<OGLTexture3D> color;
    std::shared_ptr<OGLTexture3D> depth;
};
}