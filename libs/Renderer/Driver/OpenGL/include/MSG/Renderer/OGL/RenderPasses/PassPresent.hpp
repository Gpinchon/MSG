#include <MSG/Renderer/RenderPassInterface.hpp>

#include <MSG/OGLPipelineInfo.hpp>
#include <MSG/OGLRenderPassInfo.hpp>

#include <glm/vec3.hpp>

#include <memory>

namespace MSG {
class OGLTexture3D;
class OGLFrameBuffer;
}

namespace MSG::Renderer {
class PassPresent : public RenderPassInterface {
public:
    PassPresent(Renderer::Impl& a_Renderer);
    void Update(Renderer::Impl& a_Renderer, const RenderPassesLibrary& a_RenderPasses) override;
    void Render(Impl& a_Renderer) override;
    OGLRenderPassInfo renderPassInfo;
    OGLShaderState shader;
    std::shared_ptr<OGLFrameBuffer> output;
};
}