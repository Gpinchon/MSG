#include <MSG/Renderer/RenderPassInterface.hpp>

#include <MSG/OGLRenderPassInfo.hpp>

#include <glm/vec3.hpp>

#include <memory>

namespace Msg {
class OGLFrameBuffer;
}

namespace Msg::Renderer {
class PassOpaqueGeometry : public RenderPassInterface {
public:
    PassOpaqueGeometry(Renderer::Impl& a_Renderer);
    void Update(Renderer::Impl& a_Renderer, const RenderPassesLibrary& a_Subsystems) override;
    void Render(Impl& a_Renderer) override;
    OGLRenderPassInfo renderPassInfo;
    std::shared_ptr<OGLFrameBuffer> output;
};
}