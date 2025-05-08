#pragma once
#include <MSG/OGLRenderPass.hpp>
#include <MSG/Renderer/OGL/ObjectRepertory.hpp>
#include <MSG/Renderer/OGL/RendererPath.hpp>

#include <Camera.glsl>
#include <FrameInfo.glsl>

#include <vector>

namespace MSG {
class OGLContext;
class OGLFrameBuffer;
class OGLSampler;
}

namespace MSG::Renderer {
class Impl;
struct RendererSettings;
}

namespace MSG::Renderer::RenderBuffer {
class Impl;
}

namespace MSG::Renderer {
class PathFwd : public Path {
public:
    explicit PathFwd(Renderer::Impl& a_Renderer, const RendererSettings& a_Settings);
    ~PathFwd() override = default;
    void Update(Renderer::Impl& a_Renderer) override;
    void Render(Renderer::Impl& a_Renderer) override;
    void UpdateSettings(
        Renderer::Impl& a_Renderer,
        const RendererSettings& a_Settings) override;
    void UpdateRenderBuffers(
        Renderer::Impl& a_Renderer) override;

private:
    OGLBindings _GetGlobalBindings() const;
    void _UpdateFrameInfo(Renderer::Impl& a_Renderer);
    void _UpdateCamera(Renderer::Impl& a_Renderer);
    void _UpdateLights(Renderer::Impl& a_Renderer);
    void _UpdateShadows(Renderer::Impl& a_Renderer);
    void _UpdateFog(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassOpaque(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassBlended(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassTemporalAccumulation(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassPresent(Renderer::Impl& a_Renderer);

    float _internalRes = 0;
    std::shared_ptr<OGLSampler> _TAASampler;
    ObjectRepertory<std::shared_ptr<OGLProgram>> _shaders;
    OGLShaderState _shaderCompositing;
    OGLShaderState _shaderTemporalAccumulation;
    OGLShaderState _shaderBloom;
    OGLShaderState _shaderPresent;
    std::shared_ptr<OGLVertexArray> _presentVAO;
    std::shared_ptr<OGLFrameBuffer> _fbOpaque;
    std::shared_ptr<OGLFrameBuffer> _fbBlended;
    std::shared_ptr<OGLFrameBuffer> _fbCompositing;
    std::shared_ptr<OGLFrameBuffer> _fbTemporalAccumulation[2];
    std::shared_ptr<OGLFrameBuffer> _fbPresent;
    OGLRenderPassInfo _renderPassOpaqueInfo;
    OGLRenderPassInfo _renderPassBlendedInfo;
    OGLRenderPassInfo _renderPassCompositingInfo;
    OGLRenderPassInfo _renderPassFogInfo;
    OGLRenderPassInfo _renderPassTemporalAccumulationInfo;
    OGLRenderPassInfo _renderPassPresentInfo;
};
}
