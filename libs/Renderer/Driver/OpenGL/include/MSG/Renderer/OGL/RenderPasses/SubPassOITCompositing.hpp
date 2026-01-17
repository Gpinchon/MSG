#pragma once

#include <MSG/Renderer/RenderSubPassInterface.hpp>

#include <memory>

namespace Msg {
class OGLTexture3D;
class OGLProgram;
}

namespace Msg::Renderer {
class SubPassOITCompositing : public RenderSubPassInterface {
public:
    SubPassOITCompositing(Renderer::Impl& a_Renderer);
    void Update(Renderer::Impl& a_Renderer, RenderPassInterface* a_ParentPass) override;
    void Render(Impl& a_Renderer) override;
    std::shared_ptr<OGLProgram> shader;
    std::shared_ptr<OGLTexture3D> velocity;
    std::shared_ptr<OGLTexture3D> gBuffer0;
    std::shared_ptr<OGLTexture3D> gBuffer1;
    std::shared_ptr<OGLTexture3D> depth;
};
}