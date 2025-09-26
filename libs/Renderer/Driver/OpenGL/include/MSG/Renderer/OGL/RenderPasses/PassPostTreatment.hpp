#include <MSG/Renderer/RenderPassInterface.hpp>

#include <MSG/OGLRenderPassInfo.hpp>

#include <memory>

namespace MSG {
class OGLFrameBuffer;
}

namespace MSG::Renderer {
class PassPostTreatment : public RenderPassInterface {
public:
    PassPostTreatment(Renderer::Impl& a_Renderer);
    void Update(Renderer::Impl& a_Renderer, const RenderPassesLibrary& a_RenderPasses) override;
    void Render(Impl& a_Renderer) override;
    OGLRenderPassInfo renderPassInfo;
};
}