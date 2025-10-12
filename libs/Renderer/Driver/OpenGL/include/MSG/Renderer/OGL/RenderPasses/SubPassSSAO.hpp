#pragma once

#include <MSG/Renderer/RenderSubPassInterface.hpp>

#include <memory>

namespace Msg::Renderer::GLSL {
struct SSAOSettings;
};

namespace Msg {
class OGLFrameBuffer;
class OGLProgram;
template <typename>
class OGLTypedBuffer;
}

namespace Msg::Renderer {
class SubPassSSAO : public RenderSubPassInterface {
public:
    SubPassSSAO(Renderer::Impl& a_Renderer);
    void UpdateSettings(Renderer::Impl& a_Renderer, const Renderer::RendererSettings& a_Settings) override;
    void Update(Renderer::Impl& a_Renderer, RenderPassInterface* a_ParentPass) override;
    void Render(Impl& a_Renderer) override;
    std::shared_ptr<OGLProgram> shader;
    std::shared_ptr<OGLFrameBuffer> geometryFB;
    std::shared_ptr<OGLTypedBuffer<GLSL::SSAOSettings>> ssaoBuffer;
};
}