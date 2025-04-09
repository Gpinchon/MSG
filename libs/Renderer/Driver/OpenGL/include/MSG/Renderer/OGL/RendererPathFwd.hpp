#pragma once
#include <MSG/OGLRenderPass.hpp>
#include <MSG/Renderer/OGL/LightCullerFwd.hpp>
#include <MSG/Renderer/OGL/RendererPath.hpp>
#include <MSG/Renderer/OGL/VolumetricFog.hpp>

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

namespace MSG::Renderer {

template <typename T>
class ObjectRepertory {
public:
    auto& operator=(const T& a_Value) { return storage = a_Value; }
    auto& operator*() { return storage; }
    auto& operator->() { return &storage; }
    auto& operator[](const std::string& a_Key) { return subRepertories[a_Key]; }

private:
    T storage;
    std::unordered_map<std::string, ObjectRepertory<T>> subRepertories;
};

class PathFwd : public Path {
public:
    explicit PathFwd(Renderer::Impl& a_Renderer, const RendererSettings& a_Settings);
    ~PathFwd() override = default;
    void Update(Renderer::Impl& a_Renderer) override;
    void UpdateSettings(
        Renderer::Impl& a_Renderer,
        const RendererSettings& a_Settings) override;

private:
    OGLBindings _GetGlobalBindings() const;
    void _UpdateFrameInfo(Renderer::Impl& a_Renderer);
    void _UpdateCamera(Renderer::Impl& a_Renderer);
    void _UpdateLights(Renderer::Impl& a_Renderer);
    void _UpdateShadows(Renderer::Impl& a_Renderer);
    void _UpdateFog(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassOpaque(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassBlended(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassCompositing(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassTemporalAccumulation(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassPresent(Renderer::Impl& a_Renderer);

    LightCullerFwd _lightCuller;
    VolumetricFog _volumetricFog;
    std::shared_ptr<OGLTypedBuffer<GLSL::FrameInfo>> _frameInfoBuffer;
    std::shared_ptr<OGLTypedBuffer<GLSL::CameraUBO>> _cameraBuffer;
    std::shared_ptr<OGLSampler> _fogSampler;
    std::shared_ptr<OGLSampler> _shadowSampler;
    std::shared_ptr<OGLSampler> _TAASampler;
    std::shared_ptr<OGLSampler> _iblSpecSampler;
    std::shared_ptr<OGLSampler> _brdfLutSampler;
    std::shared_ptr<OGLTexture> _brdfLut;
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
