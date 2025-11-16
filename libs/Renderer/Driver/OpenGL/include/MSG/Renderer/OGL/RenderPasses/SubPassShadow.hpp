#pragma once

#include <MSG/Renderer/RenderSubPassInterface.hpp>

#include <MSG/OGLCmdBuffer.hpp>
#include <MSG/OGLFence.hpp>

#include <memory>

namespace Msg {
class OGLFrameBuffer;
class OGLProgram;
}

namespace Msg::Renderer {
class SubPassShadow : public RenderSubPassInterface {
public:
    SubPassShadow(Renderer::Impl& a_Renderer);
    void Update(Renderer::Impl& a_Renderer, RenderPassInterface* a_ParentPass) override;
    void UpdateSettings(Renderer::Impl& a_Renderer, const RendererSettings& a_Settings) override;
    void Render(Impl& a_Renderer) override;
    OGLCmdBuffer cmdBuffer;
    OGLFence executionFence { true };
    std::shared_ptr<OGLProgram> shader;
    std::shared_ptr<OGLFrameBuffer> geometryFB;
};
}