#include <MSG/Renderer/OGL/RenderPassInterface.hpp>

#include <MSG/OGLRenderPassInfo.hpp>

#include <memory>

namespace MSG {
class OGLFrameBuffer;
}

namespace MSG::Renderer {
class DfdFog : public RenderPassInterface {
public:
    DfdFog(Renderer::Impl& a_Renderer);
    void Update(Renderer::Impl& a_Renderer, const RenderPassesLibrary& a_RenderPasses) override;
    void Render(Impl& a_Renderer) override;
    OGLRenderPassInfo renderPassInfo;
    std::shared_ptr<OGLFrameBuffer> geometryFB;
};
}