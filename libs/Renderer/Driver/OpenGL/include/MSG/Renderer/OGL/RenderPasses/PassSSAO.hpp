#include <MSG/Renderer/RenderPassInterface.hpp>

#include <MSG/OGLRenderPassInfo.hpp>

#include <memory>

namespace MSG::Renderer::GLSL {
struct SSAOSettings;
};

namespace MSG {
class OGLFrameBuffer;
template <typename>
class OGLTypedBuffer;
}

namespace MSG::Renderer {
class PassSSAO : public RenderPassInterface {
public:
    PassSSAO(Renderer::Impl& a_Renderer);
    void UpdateSettings(Renderer::Impl& a_Renderer, const Renderer::RendererSettings& a_Settings) override;
    void Update(Renderer::Impl& a_Renderer, const RenderPassesLibrary& a_RenderPasses) override;
    void Render(Impl& a_Renderer) override;
    OGLRenderPassInfo renderPassInfo;
    std::shared_ptr<OGLFrameBuffer> geometryFB;
    std::shared_ptr<OGLTypedBuffer<GLSL::SSAOSettings>> ssaoBuffer;
};
}