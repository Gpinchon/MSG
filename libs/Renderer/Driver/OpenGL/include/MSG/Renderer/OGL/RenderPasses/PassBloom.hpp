#pragma once

#include <MSG/Renderer/RenderPassInterface.hpp>

#include <memory>

namespace Msg {
class OGLFrameBuffer;
class OGLProgram;
class OGLTexture;
template <typename>
class OGLTypedBuffer;
}

namespace Msg::Renderer::GLSL {
struct BloomSettings;
}

namespace Msg::Renderer {
class PassBloom : public RenderPassInterface {
public:
    PassBloom(Renderer::Impl& a_Renderer);
    void UpdateSettings(Renderer::Impl& a_Renderer, const Renderer::RendererSettings& a_Settings) override;
    void Update(Renderer::Impl& a_Renderer, const RenderPassesLibrary& a_RenderPasses) override;
    void Render(Impl& a_Renderer) override;
    std::shared_ptr<OGLTypedBuffer<GLSL::BloomSettings>> settingsBuffer;
    std::shared_ptr<OGLProgram> compositingShader;
    std::shared_ptr<OGLProgram> lightExtractionShader;
    std::shared_ptr<OGLTexture> lightTexture;
    std::shared_ptr<OGLFrameBuffer> lightExtractionFB;
    std::shared_ptr<OGLFrameBuffer> geometryFB;
};
}