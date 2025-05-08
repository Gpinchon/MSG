#pragma once
#include <MSG/OGLCmdBuffer.hpp>
#include <MSG/OGLRenderPass.hpp>
#include <MSG/Renderer/OGL/RendererPath.hpp>

#include <SSAO.glsl>

#include <vector>

namespace MSG {
class OGLContext;
class OGLFrameBuffer;
class OGLSampler;
class OGLTexture3D;
}

namespace MSG::Renderer {
class Impl;
struct RendererSettings;
}

namespace MSG::Renderer::RenderBuffer {
class Impl;
}

namespace MSG::Renderer {
class PathDfd : public Path {
public:
    explicit PathDfd(Renderer::Impl& a_Renderer, const RendererSettings& a_Settings);
    ~PathDfd() override = default;
    void Update(Renderer::Impl& a_Renderer) override;
    void Render(Renderer::Impl& a_Renderer) override;
    void UpdateSettings(
        Renderer::Impl& a_Renderer,
        const RendererSettings& a_Settings) override;
    void UpdateRenderBuffers(
        Renderer::Impl& a_Renderer) override;

private:
    void _UpdateRenderPassGeometry(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassLight(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassOIT(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassTemporalAccumulation(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassPresent(Renderer::Impl& a_Renderer);

    OGLFence executionFence = { true };
    OGLCmdBuffer cmdBuffer;

    float _internalRes = 0;
    std::shared_ptr<OGLTypedBuffer<GLSL::SSAOSettings>> _ssaoBuffer;
    std::shared_ptr<OGLSampler> _TAASampler;
    OGLShaderState _shaderTemporalAccumulation;
    OGLShaderState _shaderBloom;
    OGLShaderState _shaderPresent;
    std::shared_ptr<OGLVertexArray> _presentVAO;
    std::shared_ptr<OGLFrameBuffer> _fbGeometry;
    std::shared_ptr<OGLFrameBuffer> _fbLightPass;
    std::shared_ptr<OGLFrameBuffer> _fbTemporalAccumulation[2];
    std::shared_ptr<OGLFrameBuffer> _fbPresent;

    OGLRenderPassInfo _renderPassGeometryInfo;
    OGLRenderPassInfo _renderPassLightInfo;
    OGLRenderPassInfo _renderPassFogInfo;
    OGLRenderPassInfo _renderPassTemporalAccumulationInfo;
    OGLRenderPassInfo _renderPassPresentInfo;

    OGLRenderPassInfo _renderPassOITInfo;
    OGLRenderPassInfo _renderPassOITCompositingInfo;
    std::shared_ptr<OGLFrameBuffer> _fbOIT;
    std::shared_ptr<OGLTexture3D> _OITColors;
    std::shared_ptr<OGLTexture3D> _OITDepth;
};
}
