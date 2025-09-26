#include <MSG/Renderer/RenderPassInterface.hpp>

#include <MSG/OGLRenderPassInfo.hpp>

#include <glm/vec3.hpp>

#include <memory>

namespace MSG {
class OGLTexture3D;
class OGLFrameBuffer;
class OGLProgram;
class OGLSampler;
}

namespace MSG::Renderer {
class TAA : public RenderPassInterface {
public:
    TAA(Renderer::Impl& a_Renderer);
    void Update(Renderer::Impl& a_Renderer, const RenderPassesLibrary& a_RenderPasses) override;
    void Render(Impl& a_Renderer) override;
    bool firstFrame = true;
    OGLRenderPassInfo renderPassInfo;
    std::shared_ptr<OGLProgram> shader;
    std::shared_ptr<OGLSampler> sampler;
    std::shared_ptr<OGLFrameBuffer> geometryFB;
    std::shared_ptr<OGLFrameBuffer> frameBuffers[2];
    std::shared_ptr<OGLFrameBuffer> output;
    std::shared_ptr<OGLFrameBuffer> output_Previous;
};
}