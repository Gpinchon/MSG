#include <MSG/Renderer/OGL/RenderPassInterface.hpp>

#include <MSG/OGLRenderPassInfo.hpp>

#include <glm/vec3.hpp>

#include <memory>

namespace MSG {
class OGLTexture3D;
class OGLFrameBuffer;
}

namespace MSG::Renderer {
class DfdBlendedGeometry : public RenderPassInterface {
public:
    DfdBlendedGeometry(Renderer::Impl& a_Renderer);
    void Update(Renderer::Impl& a_Renderer, const RenderPassesLibrary& a_Subsystems) override;
    void Render(Impl& a_Renderer) override;
    OGLRenderPassInfo renderPassInfo;
    std::shared_ptr<OGLFrameBuffer> output;
};
}