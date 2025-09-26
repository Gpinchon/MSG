#pragma once

#include <MSG/Renderer/RenderSubPassInterface.hpp>

#include <memory>

namespace MSG {
class OGLFrameBuffer;
class OGLProgram;
}

namespace MSG::Renderer {
class SubPassShadow : public RenderSubPassInterface {
public:
    SubPassShadow();
    void Update(Renderer::Impl& a_Renderer, RenderPassInterface* a_ParentPass) override;
    void UpdateSettings(Renderer::Impl& a_Renderer, const RendererSettings& a_Settings) override;
    void Render(Impl& a_Renderer) override;
    std::shared_ptr<OGLProgram> shader;
    std::shared_ptr<OGLFrameBuffer> geometryFB;
};
}