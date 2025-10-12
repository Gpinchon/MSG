#include <MSG/Renderer/RenderPassInterface.hpp>

#include <MSG/OGLRenderPassInfo.hpp>

#include <memory>

namespace Msg {
class OGLFrameBuffer;
}

namespace Msg::Renderer {
class PassLight : public RenderPassInterface {
public:
    PassLight(Renderer::Impl& a_Renderer);
    void Update(Renderer::Impl& a_Renderer, const RenderPassesLibrary& a_Subsystems) override;
    void Render(Impl& a_Renderer) override;
    OGLRenderPassInfo renderPassInfo;
    std::shared_ptr<OGLFrameBuffer> output;
};
}