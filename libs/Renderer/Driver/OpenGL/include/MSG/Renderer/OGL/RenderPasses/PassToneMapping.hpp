#include <MSG/Renderer/RenderPassInterface.hpp>

#include <MSG/OGLCmdBuffer.hpp>

#include <chrono>
#include <memory>

namespace Msg {
class OGLFrameBuffer;
class OGLBuffer;
template <typename>
class OGLTypedBuffer;
class OGLTexture;
}

namespace Msg::Renderer {
struct ToneMappingShaderSettings;
}

namespace Msg::Renderer::GLSL {
struct AutoExposureSettings;
struct ToneMappingSettings;
}

namespace Msg::Renderer {
class PassToneMapping : public RenderPassInterface {
public:
    PassToneMapping(Renderer::Impl& a_Renderer);
    void Update(Renderer::Impl& a_Renderer, const RenderPassesLibrary& a_Subsystems) override;
    void Render(Impl& a_Renderer) override;

private:
    void _ComputeAvgLum(const std::shared_ptr<OGLTexture>& a_Tgt);
    std::chrono::time_point<std::chrono::steady_clock> lastHistUpdate = std::chrono::steady_clock::now();
    std::chrono::time_point<std::chrono::steady_clock> lastUpdate     = std::chrono::steady_clock::now();
    OGLCmdBuffer cmdBuffer;
    std::shared_ptr<OGLFrameBuffer> luminanceExtractionFB;
    std::shared_ptr<OGLFrameBuffer> toneMappingFB;
    std::shared_ptr<OGLTexture> luminanceTex;
    std::shared_ptr<OGLBuffer> luminance;
    std::shared_ptr<OGLTypedBuffer<ToneMappingShaderSettings>> shaderSettingsBuffer;
};
}