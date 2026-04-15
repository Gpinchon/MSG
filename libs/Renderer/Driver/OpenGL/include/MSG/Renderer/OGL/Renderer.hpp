#pragma once

#include <MSG/ModulesLibrary.hpp>
#include <MSG/OGLCmdBuffer.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLFence.hpp>
#include <MSG/Renderer/Handles.hpp>
#include <MSG/Renderer/OGL/Loader/SamplerLoader.hpp>
#include <MSG/Renderer/OGL/Loader/TextureLoader.hpp>
#include <MSG/Renderer/OGL/Loader/VTLoader.hpp>
#include <MSG/Renderer/OGL/ObjectRepertory.hpp>
#include <MSG/Renderer/OGL/ShaderCompiler.hpp>
#include <MSG/Renderer/OGL/TextureBlurHelper.hpp>
#include <MSG/Renderer/RenderPassInterface.hpp>
#include <MSG/Renderer/Structs.hpp>
#include <MSG/Renderer/SubsystemInterface.hpp>
#include <MSG/Tools/ObjectCache.hpp>

#include <Camera.glsl>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>

namespace Msg {
class Scene;
class Texture;
class OGLVertexArray;
}

namespace Msg::Renderer {
struct CreateRendererInfo;
}

namespace Msg::Renderer {
class Impl {
public:
    Impl(const CreateRendererInfo& a_Info, const RendererSettings& a_Settings);
    void Render();
    void Update();
    void SetSettings(const RendererSettings& a_Settings);
    void SetActiveRenderBuffer(const RenderBuffer::Handle& a_RenderBuffer);
    std::shared_ptr<OGLTexture> LoadTexture(Msg::Texture* a_Texture, const bool& a_Sparse = false);
    std::shared_ptr<OGLSampler> LoadSampler(Msg::Sampler* a_Sampler);

    OGLContext context;
    OGLCmdBuffer renderCmdBuffer;
    OGLFence renderFence;

    RendererSettings settings;
    uint64_t frameIndex = 0;
    uint32_t version;
    std::string name;

    ShaderCompiler shaderCompiler;
    TextureLoader textureLoader;
    VTLoader vtLoader;
    SamplerLoader samplerLoader;

    ModulesLibrary<SubsystemInterface> subsystemsLibrary;
    ModulesLibrary<RenderPassInterface> renderPassesLibrary;

    RenderBuffer::Handle activeRenderBuffer = nullptr;
    Scene* activeScene                      = nullptr;

    // useful tool for fullscreen draws
    TextureBlurHelpers blurHelpers;
    std::shared_ptr<OGLVertexArray> presentVAO;
};
}
