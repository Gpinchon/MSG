#include <MSG/BRDF.hpp>
#include <MSG/Entity/Camera.hpp>
#include <MSG/Mesh.hpp>
#include <MSG/OGLBuffer.hpp>
#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLSampler.hpp>
#include <MSG/OGLTexture2D.hpp>
#include <MSG/OGLTexture3D.hpp>
#include <MSG/OGLTextureCube.hpp>
#include <MSG/OGLVertexArray.hpp>
#include <MSG/Renderer/OGL/Components/LightData.hpp>
#include <MSG/Renderer/OGL/Components/Mesh.hpp>
#include <MSG/Renderer/OGL/Components/MeshSkin.hpp>
#include <MSG/Renderer/OGL/Components/Transform.hpp>
#include <MSG/Renderer/OGL/Primitive.hpp>
#include <MSG/Renderer/OGL/RenderBuffer.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/RendererPathDfd.hpp>
#include <MSG/Scene.hpp>
#include <MSG/Texture.hpp>
#include <MSG/Tools/Halton.hpp>
#include <MSG/Tools/ScopedTimer.hpp>
#include <MSG/Transform.hpp>

#include <MSG/Renderer/OGL/Subsystems/MeshSubsystem.hpp>

#include <Bindings.glsl>
#include <OIT.glsl>

#include <glm/gtc/matrix_inverse.hpp>

#include <ranges>
#include <unordered_set>
#include <vector>

namespace MSG::Renderer {
static inline auto CreateFbGeometry(
    OGLContext& a_Context,
    const glm::uvec2& a_Size)
{
    auto depthStencilTexture = std::make_shared<OGLTexture2D>(a_Context, OGLTexture2DInfo { .width = a_Size.x, .height = a_Size.y, .levels = 1, .sizedFormat = GL_DEPTH24_STENCIL8 });
    OGLFrameBufferCreateInfo info;
    info.defaultSize = { a_Size, 1 };
    info.colorBuffers.resize(OUTPUT_FRAG_DFD_COUNT);
    info.colorBuffers[OUTPUT_FRAG_DFD_GBUFFER0].attachment = GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_DFD_GBUFFER0;
    info.colorBuffers[OUTPUT_FRAG_DFD_GBUFFER1].attachment = GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_DFD_GBUFFER1;
    info.colorBuffers[OUTPUT_FRAG_DFD_VELOCITY].attachment = GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_DFD_VELOCITY;
    info.colorBuffers[OUTPUT_FRAG_DFD_FINAL].attachment    = GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_DFD_FINAL;
    info.colorBuffers[OUTPUT_FRAG_DFD_GBUFFER0].texture    = std::make_shared<OGLTexture2D>(a_Context, OGLTexture2DInfo { .width = a_Size.x, .height = a_Size.y, .levels = 1, .sizedFormat = GL_RGBA32UI });
    info.colorBuffers[OUTPUT_FRAG_DFD_GBUFFER1].texture    = std::make_shared<OGLTexture2D>(a_Context, OGLTexture2DInfo { .width = a_Size.x, .height = a_Size.y, .levels = 1, .sizedFormat = GL_RGBA32UI });
    info.colorBuffers[OUTPUT_FRAG_DFD_VELOCITY].texture    = std::make_shared<OGLTexture2D>(a_Context, OGLTexture2DInfo { .width = a_Size.x, .height = a_Size.y, .levels = 1, .sizedFormat = GL_RG16F });
    info.colorBuffers[OUTPUT_FRAG_DFD_FINAL].texture       = std::make_shared<OGLTexture2D>(a_Context, OGLTexture2DInfo { .width = a_Size.x, .height = a_Size.y, .levels = 1, .sizedFormat = GL_RGBA16F });
    info.depthBuffer.texture                               = depthStencilTexture;
    info.stencilBuffer.texture                             = depthStencilTexture;
    return std::make_shared<OGLFrameBuffer>(a_Context, info);
}

static inline auto CreateFbCompositing(
    OGLContext& a_Context,
    const glm::uvec2& a_Size,
    const std::shared_ptr<OGLTexture>& a_OpaqueColor)
{
    OGLFrameBufferCreateInfo info;
    info.defaultSize = { a_Size, 1 };
    info.colorBuffers.resize(1);
    info.colorBuffers[OUTPUT_FRAG_FWD_COMP_COLOR].attachment = GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_FWD_COMP_COLOR;
    info.colorBuffers[OUTPUT_FRAG_FWD_COMP_COLOR].texture    = a_OpaqueColor;
    return std::make_shared<OGLFrameBuffer>(a_Context, info);
}

static inline auto CreateFbTemporalAccumulation(
    OGLContext& a_Context,
    const glm::uvec2& a_Size)
{
    OGLFrameBufferCreateInfo info;
    info.defaultSize = { a_Size, 1 };
    info.colorBuffers.resize(1);
    info.colorBuffers[0].attachment = GL_COLOR_ATTACHMENT0 + 0;
    info.colorBuffers[0].texture    = std::make_shared<OGLTexture2D>(a_Context, OGLTexture2DInfo { .width = a_Size.x, .height = a_Size.y, .levels = 1, .sizedFormat = GL_RGBA16F });
    return std::make_shared<OGLFrameBuffer>(a_Context, info);
}

static inline auto CreateFbPresent(
    OGLContext& a_Context,
    const glm::uvec2& a_Size)
{
    OGLFrameBufferCreateInfo info;
    info.defaultSize = { a_Size, 1 };
    return std::make_shared<OGLFrameBuffer>(a_Context, info);
}

PathDfd::PathDfd(Renderer::Impl& a_Renderer, const RendererSettings& a_Settings)
    : cmdBuffer(a_Renderer.context)
    , _ssaoBuffer(std::make_shared<OGLTypedBuffer<GLSL::SSAOSettings>>(a_Renderer.context))
    , _TAASampler(std::make_shared<OGLSampler>(a_Renderer.context, OGLSamplerParameters { .minFilter = GL_LINEAR, .wrapS = GL_CLAMP_TO_EDGE, .wrapT = GL_CLAMP_TO_EDGE, .wrapR = GL_CLAMP_TO_EDGE }))
    , _shaderTemporalAccumulation({ .program = a_Renderer.shaderCompiler.CompileProgram("TemporalAccumulation") })
    , _shaderPresent({ .program = a_Renderer.shaderCompiler.CompileProgram("Present") })
{
}

void PathDfd::Update(Renderer::Impl& a_Renderer)
{
    auto& activeScene = *a_Renderer.activeScene;
    auto& registry    = *activeScene.GetRegistry();
    executionFence.Wait();
    cmdBuffer.Reset();
    cmdBuffer.Begin();
    _UpdateRenderPassGeometry(a_Renderer);
    _UpdateRenderPassLight(a_Renderer);
    _UpdateRenderPassOIT(a_Renderer);
    _UpdateRenderPassTemporalAccumulation(a_Renderer);
    _UpdateRenderPassPresent(a_Renderer);
    cmdBuffer.End();
}

void MSG::Renderer::PathDfd::Render(Renderer::Impl& a_Renderer)
{
    executionFence.Wait();
    executionFence.Reset();
    cmdBuffer.Execute(&executionFence);
}

void PathDfd::UpdateSettings(Renderer::Impl& a_Renderer, const Renderer::RendererSettings& a_Settings)
{
    GLSL::SSAOSettings glslSSAOSettings = _ssaoBuffer->Get();
    glslSSAOSettings.radius             = a_Settings.ssao.radius;
    glslSSAOSettings.strength           = a_Settings.ssao.strength;
    _ssaoBuffer->Set(glslSSAOSettings);
    _ssaoBuffer->Update();
    _internalRes = a_Settings.internalResolution;
    UpdateRenderBuffers(a_Renderer);
}

constexpr OGLColorBlendAttachmentState GetOITBlending()
{
    return {
        .index               = OUTPUT_FRAG_FWD_COMP_COLOR,
        .enableBlend         = true,
        .srcColorBlendFactor = GL_ONE,
        .dstColorBlendFactor = GL_ONE_MINUS_SRC_ALPHA,
        .srcAlphaBlendFactor = GL_SRC_ALPHA,
        .dstAlphaBlendFactor = GL_ONE
    };
}

void PathDfd::UpdateRenderBuffers(Renderer::Impl& a_Renderer)
{
    if (a_Renderer.activeRenderBuffer == nullptr)
        return;
    auto& activeScene       = *a_Renderer.activeScene;
    auto& clearColor        = activeScene.GetBackgroundColor();
    auto& renderBuffer      = *a_Renderer.activeRenderBuffer;
    auto renderBufferSize   = glm::uvec3(renderBuffer->width, renderBuffer->height, 1);
    glm::uvec3 internalSize = glm::uvec3(glm::vec2(renderBufferSize) * _internalRes, 1);
    // UPDATE GEOMETRY RENDER BUFFER
    {
        auto fbSize = _fbGeometry != nullptr ? _fbGeometry->info.defaultSize : glm::uvec3(0);
        if (fbSize != internalSize) {
            _fbGeometry = CreateFbGeometry(a_Renderer.context, internalSize);
            // FILL VIEWPORT STATES
            auto& info                        = _renderPassGeometryInfo;
            info.name                         = "DfdGeometry";
            info.viewportState.viewport       = internalSize;
            info.viewportState.scissorExtent  = internalSize;
            info.frameBufferState.framebuffer = _fbGeometry;
            info.frameBufferState.clear.colors.resize(OUTPUT_FRAG_DFD_COUNT);
            info.frameBufferState.clear.colors[OUTPUT_FRAG_DFD_GBUFFER0] = { OUTPUT_FRAG_DFD_GBUFFER0, { 0, 0, 0, 0 } };
            info.frameBufferState.clear.colors[OUTPUT_FRAG_DFD_GBUFFER1] = { OUTPUT_FRAG_DFD_GBUFFER1, { 0, 0, 0, 0 } };
            info.frameBufferState.clear.colors[OUTPUT_FRAG_DFD_VELOCITY] = { OUTPUT_FRAG_DFD_VELOCITY, { 0, 0, 0, 0 } };
            info.frameBufferState.clear.colors[OUTPUT_FRAG_DFD_FINAL]    = { OUTPUT_FRAG_DFD_FINAL, { clearColor.r, clearColor.g, clearColor.b } };
            info.frameBufferState.clear.depthStencil                     = 0xffffff00u;
            info.frameBufferState.drawBuffers                            = {
                GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_DFD_GBUFFER0,
                GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_DFD_GBUFFER1,
                GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_DFD_VELOCITY,
                GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_DFD_FINAL
            };
        }
    }
    // UPDATE LIGHT PASS RENDER BUFFER
    {
        auto fbSize = _fbLightPass != nullptr ? _fbLightPass->info.defaultSize : glm::uvec3(0);
        if (fbSize != internalSize) {
            OGLFrameBufferCreateInfo fbInfo;
            fbInfo.defaultSize = internalSize;
            fbInfo.colorBuffers.resize(1);
            fbInfo.colorBuffers[0].attachment = GL_COLOR_ATTACHMENT0;
            fbInfo.colorBuffers[0].texture    = _fbGeometry->info.colorBuffers[OUTPUT_FRAG_DFD_FINAL].texture;
            fbInfo.stencilBuffer              = _fbGeometry->info.stencilBuffer;
            _fbLightPass                      = std::make_shared<OGLFrameBuffer>(a_Renderer.context, fbInfo);
            // FILL VIEWPORT STATES
            auto& info                        = _renderPassLightInfo;
            info.name                         = "Present";
            info.viewportState.viewport       = internalSize;
            info.viewportState.scissorExtent  = internalSize;
            info.frameBufferState.framebuffer = _fbLightPass;
            info.frameBufferState.drawBuffers = {
                GL_COLOR_ATTACHMENT0
            };
        }
    }
    // UPDATE OIT FORWARD
    {
        auto fbSize = _fbOIT != nullptr ? _fbOIT->info.defaultSize : glm::uvec3(0);
        if (fbSize != internalSize) {
            _OITDepth = std::make_shared<OGLTexture3D>(
                a_Renderer.context,
                OGLTexture3DInfo {
                    .width       = internalSize.x,
                    .height      = internalSize.y,
                    .depth       = OIT_LAYERS,
                    .sizedFormat = GL_R32UI,
                });
            _OITColors = std::make_shared<OGLTexture3D>(
                a_Renderer.context,
                OGLTexture3DInfo {
                    .width       = internalSize.x,
                    .height      = internalSize.y,
                    .depth       = OIT_LAYERS,
                    .sizedFormat = GL_RGBA16F,
                });
            OGLFrameBufferCreateInfo fbInfo;
            fbInfo.colorBuffers.resize(1);
            fbInfo.defaultSize                = internalSize;
            fbInfo.colorBuffers[0].attachment = GL_COLOR_ATTACHMENT0;
            fbInfo.colorBuffers[0].texture    = _fbGeometry->info.colorBuffers[OUTPUT_FRAG_DFD_FINAL].texture;
            fbInfo.depthBuffer                = _fbGeometry->info.depthBuffer;
            _fbOIT                            = std::make_shared<OGLFrameBuffer>(a_Renderer.context, fbInfo);
            {
                auto& info                        = _renderPassOITInfo;
                info.name                         = "OIT";
                info.viewportState.viewport       = internalSize;
                info.viewportState.scissorExtent  = internalSize;
                info.frameBufferState.framebuffer = _fbOIT;
                info.frameBufferState.drawBuffers = { GL_COLOR_ATTACHMENT0 };
            }
            {
                auto& info                        = _renderPassOITCompositingInfo;
                info.name                         = "OITCompositing";
                info.viewportState.viewport       = internalSize;
                info.viewportState.scissorExtent  = internalSize;
                info.frameBufferState.framebuffer = _fbOIT;
                info.frameBufferState.drawBuffers = { GL_COLOR_ATTACHMENT0 };
            }
        }
    }
    // UPDATE TEMPORAL RENDER BUFFER
    {
        auto fbTemporalAccumulationSize = _fbTemporalAccumulation[0] != nullptr ? _fbTemporalAccumulation[0]->info.defaultSize : glm::uvec3(0);
        if (fbTemporalAccumulationSize != renderBufferSize) {
            _fbTemporalAccumulation[0]        = CreateFbTemporalAccumulation(a_Renderer.context, renderBufferSize);
            _fbTemporalAccumulation[1]        = CreateFbTemporalAccumulation(a_Renderer.context, renderBufferSize);
            auto& info                        = _renderPassTemporalAccumulationInfo;
            info.name                         = "TemporalAccumulation";
            info.viewportState.viewport       = renderBufferSize;
            info.viewportState.scissorExtent  = renderBufferSize;
            info.frameBufferState.drawBuffers = { GL_COLOR_ATTACHMENT0 };
        }
    }
    // UPDATE PRESENT RENDER BUFFER
    {
        auto fbPresentSize = _fbPresent != nullptr ? _fbPresent->info.defaultSize : glm::uvec3(0);
        if (fbPresentSize != renderBufferSize) {
            _fbPresent = CreateFbPresent(a_Renderer.context, renderBufferSize);
            // FILL VIEWPORT STATES
            auto& info                        = _renderPassPresentInfo;
            info.name                         = "Present";
            info.viewportState.viewport       = renderBufferSize;
            info.viewportState.scissorExtent  = renderBufferSize;
            info.frameBufferState             = { .framebuffer = _fbPresent };
            info.frameBufferState.drawBuffers = {};
        }
    }
}

void PathDfd::_UpdateRenderPassGeometry(Renderer::Impl& a_Renderer)
{
    auto& meshSubsystem = a_Renderer.subsystemsLibrary.Get<MeshSubsystem>();
    auto& activeScene   = *a_Renderer.activeScene;
    auto& registry      = *activeScene.GetRegistry();
    cmdBuffer.PushCmd<OGLCmdPushRenderPass>(_renderPassGeometryInfo);
    // RENDER SKYBOX IF NEEDED
    if (activeScene.GetSkybox().texture != nullptr) {
        auto skybox  = a_Renderer.LoadTexture(activeScene.GetSkybox().texture.get());
        auto sampler = activeScene.GetSkybox().sampler != nullptr ? a_Renderer.LoadSampler(activeScene.GetSkybox().sampler.get()) : nullptr;
        auto& shader = *a_Renderer.shaderCache["DeferredSkybox"];
        if (!shader)
            shader = a_Renderer.shaderCompiler.CompileProgram("DeferredSkybox");
        OGLGraphicsPipelineInfo gpInfo;
        gpInfo.shaderState.program                 = shader;
        gpInfo.vertexInputState                    = { .vertexCount = 3, .vertexArray = a_Renderer.presentVAO };
        gpInfo.inputAssemblyState                  = { .primitiveTopology = GL_TRIANGLES };
        gpInfo.depthStencilState                   = { .enableDepthTest = false };
        gpInfo.rasterizationState                  = { .cullMode = GL_NONE };
        gpInfo.bindings                            = meshSubsystem.globalBindings;
        gpInfo.bindings.textures[SAMPLERS_SKYBOX]  = { skybox, sampler };
        gpInfo.depthStencilState.enableStencilTest = false;
        OGLCmdDrawInfo drawCmd;
        drawCmd.vertexCount = 3;
        cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
        cmdBuffer.PushCmd<OGLCmdDraw>(drawCmd);
    }
    // NOW WE RENDER OPAQUE OBJECTS
    for (auto& mesh : meshSubsystem.opaque) {
        ShaderLibrary::ProgramKeywords keywords(2);
        if (mesh.isMetRough)
            keywords[0] = { "MATERIAL_TYPE", "MATERIAL_TYPE_METALLIC_ROUGHNESS" };
        else if (mesh.isSpecGloss)
            keywords[0] = { "MATERIAL_TYPE", "MATERIAL_TYPE_SPECULAR_GLOSSINESS" };
        keywords[1]  = { "MATERIAL_UNLIT", mesh.isUnlit ? "1" : "0" };
        auto& shader = *a_Renderer.shaderCache["DeferredGeometry"][keywords[0].second][keywords[1].second];
        if (!shader)
            shader = a_Renderer.shaderCompiler.CompileProgram("DeferredGeometry", keywords);
        OGLGraphicsPipelineInfo gpInfo             = mesh.pipeline;
        gpInfo.depthStencilState.enableStencilTest = true;
        gpInfo.depthStencilState.front.passOp      = GL_REPLACE;
        gpInfo.depthStencilState.front.reference   = 255;
        gpInfo.depthStencilState.back              = gpInfo.depthStencilState.front;
        gpInfo.shaderState.program                 = shader;
        cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
        cmdBuffer.PushCmd<OGLCmdDraw>(mesh.drawCmd);
    }
    cmdBuffer.PushCmd<OGLCmdEndRenderPass>();
}

void PathDfd::_UpdateRenderPassLight(Renderer::Impl& a_Renderer)
{
    auto& meshSubsystem = a_Renderer.subsystemsLibrary.Get<MeshSubsystem>();
    auto& activeScene   = *a_Renderer.activeScene;
    OGLCmdDrawInfo drawCmd;
    drawCmd.vertexCount = 3;
    cmdBuffer.PushCmd<OGLCmdPushRenderPass>(_renderPassLightInfo);
    // DO VTFS LIGHTING
    {
        auto& shader = *a_Renderer.shaderCache["DeferredVTFS"];
        if (!shader)
            shader = a_Renderer.shaderCompiler.CompileProgram("DeferredVTFS");
        OGLGraphicsPipelineInfo gpInfo;
        gpInfo.inputAssemblyState = { .primitiveTopology = GL_TRIANGLES };
        gpInfo.rasterizationState = { .cullMode = GL_NONE };
        gpInfo.vertexInputState   = { .vertexCount = 3, .vertexArray = a_Renderer.presentVAO };
        gpInfo.bindings           = meshSubsystem.globalBindings;
        gpInfo.bindings.images[0] = { _fbGeometry->info.colorBuffers[OUTPUT_FRAG_DFD_GBUFFER0].texture, GL_READ_WRITE, GL_RGBA32UI };
        gpInfo.bindings.images[1] = { _fbGeometry->info.colorBuffers[OUTPUT_FRAG_DFD_GBUFFER1].texture, GL_READ_WRITE, GL_RGBA32UI };
        gpInfo.colorBlend.attachmentStates.resize(1);
        gpInfo.shaderState.program                 = shader;
        gpInfo.depthStencilState.enableDepthTest   = false;
        gpInfo.depthStencilState.enableStencilTest = true;
        gpInfo.depthStencilState.front.compareOp   = GL_EQUAL;
        gpInfo.depthStencilState.front.reference   = 255;
        gpInfo.depthStencilState.back              = gpInfo.depthStencilState.front;
        cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
        cmdBuffer.PushCmd<OGLCmdDraw>(drawCmd);
    }
    // DO IBL LIGHTING
    {
        auto& shader = *a_Renderer.shaderCache["DeferredIBL"];
        if (!shader)
            shader = a_Renderer.shaderCompiler.CompileProgram("DeferredIBL");
        OGLGraphicsPipelineInfo gpInfo;
        gpInfo.inputAssemblyState = { .primitiveTopology = GL_TRIANGLES };
        gpInfo.rasterizationState = { .cullMode = GL_NONE };
        gpInfo.vertexInputState   = { .vertexCount = 3, .vertexArray = a_Renderer.presentVAO };
        gpInfo.bindings           = meshSubsystem.globalBindings;
        gpInfo.bindings.images[0] = { _fbGeometry->info.colorBuffers[OUTPUT_FRAG_DFD_GBUFFER0].texture, GL_READ_WRITE, GL_RGBA32UI };
        gpInfo.bindings.images[1] = { _fbGeometry->info.colorBuffers[OUTPUT_FRAG_DFD_GBUFFER1].texture, GL_READ_WRITE, GL_RGBA32UI };
        gpInfo.colorBlend.attachmentStates.resize(1);
        gpInfo.colorBlend.attachmentStates[0].enableBlend         = true;
        gpInfo.colorBlend.attachmentStates[0].srcColorBlendFactor = GL_ONE;
        gpInfo.colorBlend.attachmentStates[0].dstColorBlendFactor = GL_ONE;
        gpInfo.colorBlend.attachmentStates[0].colorBlendOp        = GL_FUNC_ADD;
        gpInfo.colorBlend.attachmentStates[0].srcAlphaBlendFactor = GL_ONE;
        gpInfo.colorBlend.attachmentStates[0].dstAlphaBlendFactor = GL_ONE;
        gpInfo.colorBlend.attachmentStates[0].alphaBlendOp        = GL_FUNC_ADD;
        gpInfo.shaderState.program                                = a_Renderer.shaderCompiler.CompileProgram("DeferredIBL");
        gpInfo.depthStencilState.enableDepthTest                  = false;
        gpInfo.depthStencilState.enableStencilTest                = true;
        gpInfo.depthStencilState.front.compareOp                  = GL_EQUAL;
        gpInfo.depthStencilState.front.reference                  = 255;
        gpInfo.depthStencilState.back                             = gpInfo.depthStencilState.front;
        cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
        cmdBuffer.PushCmd<OGLCmdDraw>(drawCmd);
    }
    // DO SHADOWS LIGHTING
    {
        const ShaderLibrary::ProgramKeywords keywords = { { "SHADOW_QUALITY", std::to_string(int(a_Renderer.shadowQuality) + 1) } };
        auto& shader                                  = *a_Renderer.shaderCache["DeferredShadows"][keywords[0].second];
        if (!shader)
            shader = a_Renderer.shaderCompiler.CompileProgram("DeferredShadows", keywords);
        OGLGraphicsPipelineInfo gpInfo;
        gpInfo.inputAssemblyState = { .primitiveTopology = GL_TRIANGLES };
        gpInfo.rasterizationState = { .cullMode = GL_NONE };
        gpInfo.vertexInputState   = { .vertexCount = 3, .vertexArray = a_Renderer.presentVAO };
        gpInfo.bindings           = meshSubsystem.globalBindings;
        gpInfo.bindings.images[0] = { _fbGeometry->info.colorBuffers[OUTPUT_FRAG_DFD_GBUFFER0].texture, GL_READ_WRITE, GL_RGBA32UI };
        gpInfo.bindings.images[1] = { _fbGeometry->info.colorBuffers[OUTPUT_FRAG_DFD_GBUFFER1].texture, GL_READ_WRITE, GL_RGBA32UI };
        gpInfo.colorBlend.attachmentStates.resize(1);
        gpInfo.colorBlend.attachmentStates[0].enableBlend         = true;
        gpInfo.colorBlend.attachmentStates[0].srcColorBlendFactor = GL_ONE;
        gpInfo.colorBlend.attachmentStates[0].dstColorBlendFactor = GL_ONE;
        gpInfo.colorBlend.attachmentStates[0].colorBlendOp        = GL_FUNC_ADD;
        gpInfo.colorBlend.attachmentStates[0].srcAlphaBlendFactor = GL_ZERO;
        gpInfo.colorBlend.attachmentStates[0].dstAlphaBlendFactor = GL_ONE;
        gpInfo.colorBlend.attachmentStates[0].alphaBlendOp        = GL_FUNC_ADD;
        gpInfo.shaderState.program                                = shader;
        gpInfo.depthStencilState.enableDepthTest                  = false;
        gpInfo.depthStencilState.enableStencilTest                = true;
        gpInfo.depthStencilState.front.compareOp                  = GL_EQUAL;
        gpInfo.depthStencilState.front.reference                  = 255;
        gpInfo.depthStencilState.back                             = gpInfo.depthStencilState.front;
        cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
        cmdBuffer.PushCmd<OGLCmdDraw>(drawCmd);
    }
    // DO SSAO
    {
        const ShaderLibrary::ProgramKeywords keywords = { { "SSAO_QUALITY", std::to_string(int(a_Renderer.shadowQuality) + 1) } };
        auto& shader                                  = *a_Renderer.shaderCache["DeferredSSAO"][keywords[0].second];
        if (!shader)
            shader = a_Renderer.shaderCompiler.CompileProgram("DeferredSSAO", keywords);
        OGLGraphicsPipelineInfo gpInfo;
        gpInfo.inputAssemblyState                      = { .primitiveTopology = GL_TRIANGLES };
        gpInfo.rasterizationState                      = { .cullMode = GL_NONE };
        gpInfo.vertexInputState                        = { .vertexCount = 3, .vertexArray = a_Renderer.presentVAO };
        gpInfo.bindings                                = meshSubsystem.globalBindings;
        gpInfo.bindings.uniformBuffers[UBO_CAMERA + 1] = { .buffer = _ssaoBuffer, .offset = 0, .size = _ssaoBuffer->size };
        gpInfo.bindings.images[0]                      = { _fbGeometry->info.colorBuffers[OUTPUT_FRAG_DFD_GBUFFER0].texture, GL_READ_ONLY, GL_RGBA32UI };
        gpInfo.bindings.images[1]                      = { _fbGeometry->info.colorBuffers[OUTPUT_FRAG_DFD_GBUFFER1].texture, GL_READ_ONLY, GL_RGBA32UI };
        gpInfo.colorBlend.attachmentStates.resize(1);
        gpInfo.colorBlend.attachmentStates[0].enableBlend         = true;
        gpInfo.colorBlend.attachmentStates[0].srcColorBlendFactor = GL_ZERO;
        gpInfo.colorBlend.attachmentStates[0].dstColorBlendFactor = GL_SRC_COLOR;
        gpInfo.colorBlend.attachmentStates[0].colorBlendOp        = GL_FUNC_ADD;
        gpInfo.colorBlend.attachmentStates[0].srcAlphaBlendFactor = GL_ZERO;
        gpInfo.colorBlend.attachmentStates[0].dstAlphaBlendFactor = GL_ONE;
        gpInfo.colorBlend.attachmentStates[0].alphaBlendOp        = GL_FUNC_ADD;
        gpInfo.shaderState.program                                = shader;
        gpInfo.depthStencilState.enableDepthTest                  = false;
        gpInfo.depthStencilState.enableStencilTest                = true;
        gpInfo.depthStencilState.front.compareOp                  = GL_EQUAL;
        gpInfo.depthStencilState.front.reference                  = 255;
        gpInfo.depthStencilState.back                             = gpInfo.depthStencilState.front;
        cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
        cmdBuffer.PushCmd<OGLCmdDraw>(drawCmd);
    }
    // DO FOG
    {
        auto& shader = *a_Renderer.shaderCache["DeferredFog"];
        if (!shader)
            shader = a_Renderer.shaderCompiler.CompileProgram("DeferredFog");
        OGLGraphicsPipelineInfo gpInfo;
        gpInfo.inputAssemblyState = { .primitiveTopology = GL_TRIANGLES };
        gpInfo.rasterizationState = { .cullMode = GL_NONE };
        gpInfo.vertexInputState   = { .vertexCount = 3, .vertexArray = a_Renderer.presentVAO };
        gpInfo.bindings           = meshSubsystem.globalBindings;
        gpInfo.bindings.images[0] = { _fbGeometry->info.colorBuffers[OUTPUT_FRAG_DFD_GBUFFER0].texture, GL_READ_WRITE, GL_RGBA32UI };
        gpInfo.bindings.images[1] = { _fbGeometry->info.colorBuffers[OUTPUT_FRAG_DFD_GBUFFER1].texture, GL_READ_WRITE, GL_RGBA32UI };
        gpInfo.colorBlend.attachmentStates.resize(1);
        gpInfo.colorBlend.attachmentStates[0].enableBlend         = true;
        gpInfo.colorBlend.attachmentStates[0].srcColorBlendFactor = GL_ONE;
        gpInfo.colorBlend.attachmentStates[0].dstColorBlendFactor = GL_SRC_ALPHA;
        gpInfo.colorBlend.attachmentStates[0].colorBlendOp        = GL_FUNC_ADD;
        gpInfo.colorBlend.attachmentStates[0].srcAlphaBlendFactor = GL_ZERO;
        gpInfo.colorBlend.attachmentStates[0].dstAlphaBlendFactor = GL_ONE;
        gpInfo.colorBlend.attachmentStates[0].alphaBlendOp        = GL_FUNC_ADD;
        gpInfo.shaderState.program                                = shader;
        cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
        cmdBuffer.PushCmd<OGLCmdDraw>(drawCmd);
    }
    cmdBuffer.PushCmd<OGLCmdEndRenderPass>();
}

void PathDfd::_UpdateRenderPassOIT(Renderer::Impl& a_Renderer)
{
    auto& meshSubsystem = a_Renderer.subsystemsLibrary.Get<MeshSubsystem>();
    auto& activeScene   = *a_Renderer.activeScene;
    auto& registry      = *activeScene.GetRegistry();
    auto shadowQuality  = std::to_string(int(a_Renderer.shadowQuality) + 1);

    if (meshSubsystem.blended.empty())
        return;
    cmdBuffer.PushCmd<OGLCmdPushRenderPass>(_renderPassOITInfo);
    {
        cmdBuffer.PushCmd<OGLCmdClearTexture>(_OITDepth,
            OGLClearTextureInfo {
                .size  = { _OITDepth->width, _OITDepth->height, _OITDepth->depth },
                .value = glm::uvec4(std::numeric_limits<uint32_t>::max()),
            });
        // RENDER DEPTH
        for (auto& mesh : meshSubsystem.blended) {
            ShaderLibrary::ProgramKeywords keywords(1);
            if (mesh.isMetRough)
                keywords[0] = { "MATERIAL_TYPE", "MATERIAL_TYPE_METALLIC_ROUGHNESS" };
            else if (mesh.isSpecGloss)
                keywords[0] = { "MATERIAL_TYPE", "MATERIAL_TYPE_SPECULAR_GLOSSINESS" };
            auto& shader = *a_Renderer.shaderCache["OITDepth"][keywords[0].second];
            if (!shader)
                shader = a_Renderer.shaderCompiler.CompileProgram("OITDepth", keywords);
            OGLGraphicsPipelineInfo gpInfo            = mesh.pipeline;
            gpInfo.shaderState.program                = shader;
            gpInfo.colorBlend                         = { .attachmentStates = { GetOITBlending() } };
            gpInfo.depthStencilState.enableDepthWrite = false;
            gpInfo.bindings.images[IMG_OIT_DEPTH]     = { .texture = _OITDepth, .access = GL_READ_WRITE, .format = GL_R32UI, .layered = true };
            cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
            cmdBuffer.PushCmd<OGLCmdDraw>(mesh.drawCmd);
        }
        // RENDER SURFACES
        for (auto& mesh : meshSubsystem.blended) {
            ShaderLibrary::ProgramKeywords keywords(3);
            if (mesh.isMetRough)
                keywords[0] = { "MATERIAL_TYPE", "MATERIAL_TYPE_METALLIC_ROUGHNESS" };
            else if (mesh.isSpecGloss)
                keywords[0] = { "MATERIAL_TYPE", "MATERIAL_TYPE_SPECULAR_GLOSSINESS" };
            keywords[1]  = { "MATERIAL_UNLIT", mesh.isUnlit ? "1" : "0" };
            keywords[2]  = { "SHADOW_QUALITY", shadowQuality };
            auto& shader = *a_Renderer.shaderCache["OITForward"][keywords[0].second][keywords[1].second][keywords[2].second];
            if (!shader)
                shader = a_Renderer.shaderCompiler.CompileProgram("OITForward", keywords);
            OGLGraphicsPipelineInfo gpInfo            = mesh.pipeline;
            gpInfo.shaderState.program                = shader;
            gpInfo.colorBlend                         = { .attachmentStates = { GetOITBlending() } };
            gpInfo.depthStencilState.enableDepthWrite = false;
            gpInfo.bindings.images[IMG_OIT_COLORS]    = { .texture = _OITColors, .access = GL_WRITE_ONLY, .format = GL_RGBA16F, .layered = true };
            gpInfo.bindings.images[IMG_OIT_DEPTH]     = { .texture = _OITDepth, .access = GL_READ_ONLY, .format = GL_R32F, .layered = true };
            cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
            cmdBuffer.PushCmd<OGLCmdDraw>(mesh.drawCmd);
        }
        cmdBuffer.PushCmd<OGLCmdMemoryBarrier>(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT, true);
    }
    cmdBuffer.PushCmd<OGLCmdEndRenderPass>();
    // FILL GRAPHICS PIPELINES
    auto& shader = *a_Renderer.shaderCache["OITCompositing"];
    if (!shader)
        shader = a_Renderer.shaderCompiler.CompileProgram("OITCompositing");
    OGLGraphicsPipelineInfo gpInfo;
    gpInfo.colorBlend                      = { .attachmentStates = { GetOITBlending() } };
    gpInfo.depthStencilState               = { .enableDepthTest = false };
    gpInfo.shaderState.program             = shader;
    gpInfo.inputAssemblyState              = { .primitiveTopology = GL_TRIANGLES };
    gpInfo.rasterizationState              = { .cullMode = GL_NONE };
    gpInfo.vertexInputState                = { .vertexCount = 3, .vertexArray = a_Renderer.presentVAO };
    gpInfo.bindings.images[IMG_OIT_COLORS] = { .texture = _OITColors, .access = GL_READ_ONLY, .format = GL_RGBA16F, .layered = true };
    gpInfo.bindings.images[IMG_OIT_DEPTH]  = { .texture = _OITDepth, .access = GL_READ_ONLY, .format = GL_R32UI, .layered = true };
    OGLCmdDrawInfo drawCmd;
    drawCmd.vertexCount = 3;
    cmdBuffer.PushCmd<OGLCmdPushRenderPass>(_renderPassOITCompositingInfo);
    cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
    cmdBuffer.PushCmd<OGLCmdDraw>(drawCmd);
    cmdBuffer.PushCmd<OGLCmdEndRenderPass>();
}

void PathDfd::_UpdateRenderPassTemporalAccumulation(Renderer::Impl& a_Renderer)
{
    auto& fbTemporalAccumulation          = _fbTemporalAccumulation[(a_Renderer.frameIndex + 0) % 2];
    auto& fbTemporalAccumulation_Previous = _fbTemporalAccumulation[(a_Renderer.frameIndex + 1) % 2];
    // FILL VIEWPORT STATES
    _renderPassTemporalAccumulationInfo.frameBufferState.framebuffer = fbTemporalAccumulation;
    // FILL GRAPHICS PIPELINES
    auto color_Previous = fbTemporalAccumulation_Previous != nullptr ? fbTemporalAccumulation_Previous->info.colorBuffers[0].texture : _fbGeometry->info.colorBuffers[OUTPUT_FRAG_DFD_FINAL].texture;
    OGLGraphicsPipelineInfo gpInfo;
    gpInfo.depthStencilState    = { .enableDepthTest = false };
    gpInfo.shaderState          = _shaderTemporalAccumulation;
    gpInfo.inputAssemblyState   = { .primitiveTopology = GL_TRIANGLES };
    gpInfo.rasterizationState   = { .cullMode = GL_NONE };
    gpInfo.vertexInputState     = { .vertexCount = 3, .vertexArray = a_Renderer.presentVAO };
    gpInfo.bindings.textures[0] = { .texture = color_Previous, .sampler = _TAASampler };
    gpInfo.bindings.textures[1] = { .texture = _fbGeometry->info.colorBuffers[OUTPUT_FRAG_DFD_FINAL].texture, .sampler = _TAASampler };
    gpInfo.bindings.textures[2] = { .texture = _fbGeometry->info.colorBuffers[OUTPUT_FRAG_DFD_VELOCITY].texture, .sampler = _TAASampler };
    OGLCmdDrawInfo drawCmd;
    drawCmd.vertexCount = 3;
    cmdBuffer.PushCmd<OGLCmdPushRenderPass>(_renderPassTemporalAccumulationInfo);
    cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
    cmdBuffer.PushCmd<OGLCmdDraw>(drawCmd);
    cmdBuffer.PushCmd<OGLCmdEndRenderPass>();
}

void PathDfd::_UpdateRenderPassPresent(Renderer::Impl& a_Renderer)
{
    auto& renderBuffer           = *a_Renderer.activeRenderBuffer;
    auto& fbTemporalAccumulation = _fbTemporalAccumulation[a_Renderer.frameIndex % 2];
    auto& info                   = _renderPassPresentInfo;
    // FILL GRAPHICS PIPELINES
    OGLGraphicsPipelineInfo gpInfo;
    gpInfo.depthStencilState  = { .enableDepthTest = false };
    gpInfo.shaderState        = _shaderPresent;
    gpInfo.inputAssemblyState = { .primitiveTopology = GL_TRIANGLES };
    gpInfo.rasterizationState = { .cullMode = GL_NONE };
    gpInfo.vertexInputState   = { .vertexCount = 3, .vertexArray = a_Renderer.presentVAO };
    gpInfo.bindings.images[0] = { fbTemporalAccumulation->info.colorBuffers[0].texture, GL_READ_ONLY, GL_RGBA16F };
    gpInfo.bindings.images[1] = { renderBuffer, GL_WRITE_ONLY, GL_RGBA8 };
    OGLCmdDrawInfo drawCmd;
    drawCmd.vertexCount = 3;
    cmdBuffer.PushCmd<OGLCmdPushRenderPass>(_renderPassPresentInfo);
    cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
    cmdBuffer.PushCmd<OGLCmdDraw>(drawCmd);
    cmdBuffer.PushCmd<OGLCmdEndRenderPass>();
}
}
