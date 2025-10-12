#include <MSG/Renderer/RenderPassInterface.hpp>

#include <MSG/OGLRenderPassInfo.hpp>

#include <memory>

namespace Msg {
class OGLFrameBuffer;
}

namespace Msg::Renderer {
class PassPostTreatment : public RenderPassInterface {
public:
    PassPostTreatment(Renderer::Impl& a_Renderer);
    void Update(Renderer::Impl& a_Renderer, const RenderPassesLibrary& a_RenderPasses) override;
    void Render(Impl& a_Renderer) override;
    OGLRenderPassInfo renderPassInfo;
};
}