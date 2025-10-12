#include <MSG/Renderer/RenderSubPassInterface.hpp>

#include <MSG/OGLRenderPassInfo.hpp>

#include <memory>

namespace Msg {
class OGLFrameBuffer;
class OGLProgram;
}

namespace Msg::Renderer {
class SubPassFog : public RenderSubPassInterface {
public:
    SubPassFog(Renderer::Impl& a_Renderer);
    void Update(Renderer::Impl& a_Renderer, RenderPassInterface* a_ParentPass) override;
    void Render(Impl& a_Renderer) override;
    std::shared_ptr<OGLProgram> shader;
    std::shared_ptr<OGLFrameBuffer> geometryFB;
};
}