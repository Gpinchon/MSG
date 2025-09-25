#pragma once

#include <MSG/Renderer/OGL/RenderSubPassInterface.hpp>

#include <memory>

namespace MSG {
class OGLTexture3D;
}

namespace MSG::Renderer {
class DfdSubPassOITForward : public RenderSubPassInterface {
public:
    void Update(Renderer::Impl& a_Renderer, RenderPassInterface* a_ParentPass) override;
    void Render(Impl& a_Renderer) override;
    std::shared_ptr<OGLTexture3D> color;
    std::shared_ptr<OGLTexture3D> depth;
};
}