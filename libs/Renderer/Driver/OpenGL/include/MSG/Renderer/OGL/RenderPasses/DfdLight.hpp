#include <MSG/Renderer/RenderPassInterface.hpp>

#include <MSG/OGLRenderPassInfo.hpp>

#include <memory>

namespace MSG {
class OGLFrameBuffer;
}

namespace MSG::Renderer {
class DfdLight : public RenderPassInterface {
public:
    DfdLight(Renderer::Impl& a_Renderer);
    void Update(Renderer::Impl& a_Renderer, const RenderPassesLibrary& a_Subsystems) override;
    void Render(Impl& a_Renderer) override;
    OGLRenderPassInfo renderPassInfo;
    std::shared_ptr<OGLFrameBuffer> output;
};
}