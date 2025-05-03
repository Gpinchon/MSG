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
#include <MSG/Renderer/OGL/Material.hpp>
#include <MSG/Renderer/OGL/Primitive.hpp>
#include <MSG/Renderer/OGL/RenderBuffer.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/RendererPathDfd.hpp>
#include <MSG/Scene.hpp>
#include <MSG/Texture.hpp>
#include <MSG/Tools/Halton.hpp>
#include <MSG/Tools/ScopedTimer.hpp>
#include <MSG/Transform.hpp>

#include <Material.glsl>
#include <OIT.glsl>

#include <glm/gtc/matrix_inverse.hpp>

#include <ranges>
#include <unordered_set>
#include <vector>

namespace MSG::Renderer {
static inline auto ApplyTemporalJitter(glm::mat4 a_ProjMat, const uint8_t& a_FrameIndex)
{
    // the jitter amount should go bellow the threshold of velocity buffer
    constexpr float f16lowest = 1 / 1024.f;
    auto halton               = (Tools::Halton23<256>(a_FrameIndex) * 0.5f + 0.5f) * f16lowest;
    a_ProjMat[2][0] += halton.x;
    a_ProjMat[2][1] += halton.y;
    return a_ProjMat;
}

static inline auto CreatePresentVAO(OGLContext& a_Context)
{
    OGLVertexAttributeDescription attribDesc {};
    attribDesc.binding           = 0;
    attribDesc.format.normalized = false;
    attribDesc.format.size       = 1;
    attribDesc.format.type       = GL_BYTE;
    OGLVertexBindingDescription bindingDesc {};
    bindingDesc.buffer = std::make_shared<OGLBuffer>(a_Context, 3, nullptr, 0);
    bindingDesc.index  = 0;
    bindingDesc.offset = 0;
    bindingDesc.stride = 1;
    std::vector<OGLVertexAttributeDescription> attribs { attribDesc };
    std::vector<OGLVertexBindingDescription> bindings { bindingDesc };
    return std::make_shared<OGLVertexArray>(a_Context,
        3, attribs, bindings);
}

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

static inline auto GetCommonGraphicsPipeline(
    const OGLBindings& a_GlobalBindings,
    const Primitive& a_rPrimitive,
    const Material& a_rMaterial,
    const Component::Transform& a_rTransform,
    const Component::MeshSkin* a_rMeshSkin)
{
    OGLGraphicsPipelineInfo info;
    info.bindings                               = a_GlobalBindings;
    info.bindings.uniformBuffers[UBO_TRANSFORM] = { a_rTransform.buffer, 0, a_rTransform.buffer->size };
    info.bindings.uniformBuffers[UBO_MATERIAL]  = { a_rMaterial.buffer, 0, a_rMaterial.buffer->size };
    info.inputAssemblyState.primitiveTopology   = a_rPrimitive.drawMode;
    info.vertexInputState.vertexArray           = a_rPrimitive.vertexArray;
    info.rasterizationState.cullMode            = a_rMaterial.doubleSided ? GL_NONE : GL_BACK;
    if (a_rMeshSkin != nullptr) [[unlikely]] {
        info.bindings.storageBuffers[SSBO_MESH_SKIN]      = { a_rMeshSkin->buffer, 0, a_rMeshSkin->buffer->size };
        info.bindings.storageBuffers[SSBO_MESH_SKIN_PREV] = { a_rMeshSkin->buffer_Previous, 0, a_rMeshSkin->buffer_Previous->size };
    }
    for (uint32_t i = 0; i < a_rMaterial.textureSamplers.size(); ++i) {
        auto& textureSampler                          = a_rMaterial.textureSamplers.at(i);
        info.bindings.textures[SAMPLERS_MATERIAL + i] = { textureSampler.texture, textureSampler.sampler };
    }
    if (a_rPrimitive.vertexArray->indexed) {
        OGLDrawCommand drawCmd;
        drawCmd.indexed        = true;
        drawCmd.instanceCount  = 1;
        drawCmd.instanceOffset = 0;
        drawCmd.vertexOffset   = 0;
        // indexed specific info
        drawCmd.indexCount  = a_rPrimitive.vertexArray->indexCount;
        drawCmd.indexOffset = 0;
        info.drawCommands.emplace_back(drawCmd);
    } else {
        OGLDrawCommand drawCmd;
        drawCmd.indexed        = true;
        drawCmd.instanceCount  = 1;
        drawCmd.instanceOffset = 0;
        drawCmd.vertexOffset   = 0;
        // non indexed specific info
        drawCmd.vertexCount = a_rPrimitive.vertexArray->vertexCount;
        info.drawCommands.emplace_back(drawCmd);
    }
    return info;
}

PathDfd::PathDfd(Renderer::Impl& a_Renderer, const RendererSettings& a_Settings)
    : _volumetricFog(a_Renderer)
    , _lightCuller(a_Renderer)
    , _frameInfoBuffer(std::make_shared<OGLTypedBuffer<GLSL::FrameInfo>>(a_Renderer.context))
    , _cameraBuffer(std::make_shared<OGLTypedBuffer<GLSL::CameraUBO>>(a_Renderer.context))
    , _ssaoBuffer(std::make_shared<OGLTypedBuffer<GLSL::SSAOSettings>>(a_Renderer.context))
    , _fogSampler(std::make_shared<OGLSampler>(a_Renderer.context, OGLSamplerParameters { .minFilter = GL_LINEAR, .magFilter = GL_LINEAR, .wrapS = GL_CLAMP_TO_BORDER, .wrapT = GL_CLAMP_TO_BORDER, .wrapR = GL_CLAMP_TO_BORDER, .borderColor { 0, 0, 0, 1 } }))
    , _shadowSampler(std::make_shared<OGLSampler>(a_Renderer.context, OGLSamplerParameters { .wrapS = GL_CLAMP_TO_BORDER, .wrapT = GL_CLAMP_TO_BORDER, .wrapR = GL_CLAMP_TO_BORDER, .borderColor = { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX } }))
    //, _shadowSampler(std::make_shared<OGLSampler>(a_Renderer.context, OGLSamplerParameters { .wrapS = GL_CLAMP_TO_EDGE, .wrapT = GL_CLAMP_TO_EDGE, .wrapR = GL_CLAMP_TO_EDGE }))
    , _TAASampler(std::make_shared<OGLSampler>(a_Renderer.context, OGLSamplerParameters { .wrapS = GL_CLAMP_TO_EDGE, .wrapT = GL_CLAMP_TO_EDGE, .wrapR = GL_CLAMP_TO_EDGE }))
    , _iblSpecSampler(std::make_shared<OGLSampler>(a_Renderer.context, OGLSamplerParameters { .minFilter = GL_LINEAR_MIPMAP_LINEAR }))
    , _brdfLutSampler(std::make_shared<OGLSampler>(a_Renderer.context, OGLSamplerParameters { .wrapS = GL_CLAMP_TO_EDGE, .wrapT = GL_CLAMP_TO_EDGE, .wrapR = GL_CLAMP_TO_EDGE }))
    , _brdfLut(a_Renderer.LoadTexture(new Texture(BRDF::GenerateTexture(BRDF::Type::Standard))))
    , _shaderTemporalAccumulation({ .program = a_Renderer.shaderCompiler.CompileProgram("TemporalAccumulation") })
    , _shaderPresent({ .program = a_Renderer.shaderCompiler.CompileProgram("Present") })
    , _presentVAO(CreatePresentVAO(a_Renderer.context))
{
}

void PathDfd::Update(Renderer::Impl& a_Renderer)
{
    auto& activeScene   = *a_Renderer.activeScene;
    auto& registry      = *activeScene.GetRegistry();
    auto globalBindings = _GetGlobalBindings();
    opaqueMeshes.clear();
    blendedMeshes.clear();
    opaqueMeshes.reserve(activeScene.GetVisibleEntities().meshes.size());
    blendedMeshes.reserve(activeScene.GetVisibleEntities().meshes.size());
    for (auto& entity : activeScene.GetVisibleEntities().meshes) {
        auto& rMesh      = registry.GetComponent<Component::Mesh>(entity).at(entity.lod);
        auto& rTransform = registry.GetComponent<Component::Transform>(entity);
        auto rMeshSkin   = registry.HasComponent<Component::MeshSkin>(entity) ? &registry.GetComponent<Component::MeshSkin>(entity) : nullptr;
        for (auto& [rPrimitive, rMaterial] : rMesh) {
            const bool isAlphaBlend = rMaterial->alphaMode == MATERIAL_ALPHA_BLEND;
            MeshInfo* meshInfo;
            if (isAlphaBlend)
                meshInfo = &blendedMeshes.emplace_back();
            else
                meshInfo = &opaqueMeshes.emplace_back();
            meshInfo->pipeline    = GetCommonGraphicsPipeline(globalBindings, *rPrimitive, *rMaterial, rTransform, rMeshSkin);
            meshInfo->isMetRough  = rMaterial->type == MATERIAL_TYPE_METALLIC_ROUGHNESS;
            meshInfo->isSpecGloss = rMaterial->type == MATERIAL_TYPE_SPECULAR_GLOSSINESS;
            meshInfo->isUnlit     = rMaterial->unlit;
        }
    }

    renderPasses.clear();
    _UpdateFrameInfo(a_Renderer);
    _UpdateCamera(a_Renderer);
    _UpdateLights(a_Renderer);
    _UpdateShadows(a_Renderer);
    _UpdateFog(a_Renderer);
    _UpdateRenderPassGeometry(a_Renderer);
    _UpdateRenderPassLight(a_Renderer);
    _UpdateRenderPassOIT(a_Renderer);
    _UpdateRenderPassTemporalAccumulation(a_Renderer);
    _UpdateRenderPassPresent(a_Renderer);
}

void PathDfd::UpdateSettings(Renderer::Impl& a_Renderer, const Renderer::RendererSettings& a_Settings)
{
    GLSL::SSAOSettings glslSSAOSettings = _ssaoBuffer->Get();
    glslSSAOSettings.radius             = a_Settings.ssao.radius;
    glslSSAOSettings.strength           = a_Settings.ssao.strength;
    _ssaoBuffer->Set(glslSSAOSettings);
    _ssaoBuffer->Update();
    _volumetricFog.UpdateSettings(a_Renderer, a_Settings);
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
                // FILL GRAPHICS PIPELINES
                info.pipelines.clear();
                auto& gpInfo                                   = std::get<OGLGraphicsPipelineInfo>(info.pipelines.emplace_back());
                gpInfo.colorBlend                              = { .attachmentStates = { GetOITBlending() } };
                gpInfo.depthStencilState                       = { .enableDepthTest = false };
                gpInfo.shaderState.program                     = a_Renderer.shaderCompiler.CompileProgram("OITCompositing");
                gpInfo.inputAssemblyState                      = { .primitiveTopology = GL_TRIANGLES };
                gpInfo.rasterizationState                      = { .cullMode = GL_NONE };
                gpInfo.vertexInputState                        = { .vertexCount = 3, .vertexArray = _presentVAO };
                gpInfo.bindings.images[IMG_OIT_COLORS]         = { .texture = _OITColors, .access = GL_READ_ONLY, .format = GL_RGBA16F, .layered = true };
                gpInfo.bindings.images[IMG_OIT_DEPTH]          = { .texture = _OITDepth, .access = GL_READ_ONLY, .format = GL_R32UI, .layered = true };
                gpInfo.drawCommands.emplace_back().vertexCount = 3;
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

OGLBindings PathDfd::_GetGlobalBindings() const
{
    OGLBindings bindings;
    bindings.uniformBuffers[UBO_FRAME_INFO]     = { _frameInfoBuffer, 0, _frameInfoBuffer->size };
    bindings.uniformBuffers[UBO_CAMERA]         = { _cameraBuffer, 0, _cameraBuffer->size };
    bindings.uniformBuffers[UBO_FOG_CAMERA]     = { _volumetricFog.fogCamerasBuffer, 0, _volumetricFog.fogCamerasBuffer->size };
    bindings.uniformBuffers[UBO_FOG_SETTINGS]   = { _volumetricFog.fogSettingsBuffer, 0, _volumetricFog.fogSettingsBuffer->size };
    bindings.uniformBuffers[UBO_FWD_IBL]        = { _lightCuller.ibls.buffer, 0, _lightCuller.ibls.buffer->size };
    bindings.uniformBuffers[UBO_FWD_SHADOWS]    = { _lightCuller.shadows.buffer, 0, _lightCuller.shadows.buffer->size };
    bindings.storageBuffers[SSBO_VTFS_LIGHTS]   = { _lightCuller.vtfs.buffer.lightsBuffer, offsetof(GLSL::VTFSLightsBuffer, lights), _lightCuller.vtfs.buffer.lightsBuffer->size };
    bindings.storageBuffers[SSBO_VTFS_CLUSTERS] = { _lightCuller.vtfs.buffer.cluster, 0, _lightCuller.vtfs.buffer.cluster->size };
    bindings.textures[SAMPLERS_BRDF_LUT]        = { _brdfLut, _brdfLutSampler };
    for (uint32_t i = 0; i < _volumetricFog.textures.size(); i++)
        bindings.textures[SAMPLERS_FOG + i] = { _volumetricFog.textures[i].resultTexture, _volumetricFog.sampler };
    for (auto i = 0u; i < _lightCuller.ibls.buffer->Get().count; i++) {
        auto& texture                       = _lightCuller.ibls.textures.at(i);
        bindings.textures[SAMPLERS_IBL + i] = { .texture = texture, .sampler = _iblSpecSampler };
    }
    for (auto i = 0u; i < _lightCuller.shadows.buffer->Get().count; i++) {
        bindings.textures[SAMPLERS_SHADOW + i] = { .texture = _lightCuller.shadows.texturesMoments[i], .sampler = _shadowSampler };
    }
    return bindings;
}

void PathDfd::_UpdateFrameInfo(Renderer::Impl& a_Renderer)
{
    GLSL::FrameInfo frameInfo;
    frameInfo.width      = (*a_Renderer.activeRenderBuffer)->width;
    frameInfo.height     = (*a_Renderer.activeRenderBuffer)->height;
    frameInfo.frameIndex = a_Renderer.frameIndex;
    _frameInfoBuffer->Set(frameInfo);
    _frameInfoBuffer->Update();
}

void PathDfd::_UpdateCamera(Renderer::Impl& a_Renderer)
{
    auto& activeScene                = *a_Renderer.activeScene;
    auto& currentCamera              = activeScene.GetCamera();
    auto& camera                     = currentCamera.GetComponent<Camera>();
    GLSL::CameraUBO cameraUBOData    = _cameraBuffer->Get();
    cameraUBOData.previous           = cameraUBOData.current;
    cameraUBOData.current.position   = currentCamera.GetComponent<MSG::Transform>().GetWorldPosition();
    cameraUBOData.current.projection = camera.projection.GetMatrix();
    cameraUBOData.current.zNear      = camera.projection.GetZNear();
    cameraUBOData.current.zFar       = camera.projection.GetZFar();
    if (a_Renderer.enableTAA)
        cameraUBOData.current.projection = ApplyTemporalJitter(cameraUBOData.current.projection, uint8_t(a_Renderer.frameIndex));
    cameraUBOData.current.view = glm::inverse(currentCamera.GetComponent<MSG::Transform>().GetWorldTransformMatrix());
    _cameraBuffer->Set(cameraUBOData);
    _cameraBuffer->Update();
}

void PathDfd::_UpdateLights(Renderer::Impl& a_Renderer)
{
    auto& activeScene   = *a_Renderer.activeScene;
    auto& registry      = *activeScene.GetRegistry();
    auto& currentCamera = activeScene.GetCamera();
    std::scoped_lock lock(activeScene.GetRegistry()->GetLock());
    for (auto& entityID : activeScene.GetVisibleEntities().lights) {
        registry.GetComponent<Component::LightData>(entityID).Update(a_Renderer, registry, entityID);
    }
    for (auto& shadow : activeScene.GetVisibleEntities().shadows) {
        auto& lightTransform = registry.GetComponent<Transform>(shadow);
        auto& lightData      = registry.GetComponent<Component::LightData>(shadow);
        for (uint8_t vI = 0; vI < shadow.viewports.size(); vI++) {
            const auto& viewport = shadow.viewports.at(vI);
            const float zNear    = viewport.projection.GetZNear();
            const float zFar     = viewport.projection.GetZFar();
            GLSL::Camera proj    = lightData.shadow->projBuffer->Get(vI);
            proj.projection      = viewport.projection;
            proj.view            = viewport.viewMatrix;
            proj.position        = lightTransform.GetWorldPosition();
            proj.zNear           = zNear;
            proj.zFar            = zFar == std::numeric_limits<float>::infinity() ? 1000000.f : zFar;
            lightData.shadow->projBuffer->Set(vI, proj);
        }
        lightData.shadow->projBuffer->Update();
    }
    _lightCuller(
        a_Renderer.activeScene,
        _cameraBuffer);
}

void PathDfd::_UpdateFog(Renderer::Impl& a_Renderer)
{
    _volumetricFog.Update(a_Renderer);
    auto computePass = _volumetricFog.GetComputePass(_lightCuller, _shadowSampler, _frameInfoBuffer);
    if (computePass == nullptr)
        return; // no fog, no need to continue
    renderPasses.emplace_back(computePass);
}

void PathDfd::_UpdateShadows(Renderer::Impl& a_Renderer)
{
    auto& activeScene = *a_Renderer.activeScene;
    auto& registry    = *activeScene.GetRegistry();
    auto& shadows     = _lightCuller.shadows.buffer->Get();
    for (uint32_t i = 0; i < shadows.count; i++) {
        auto& visibleShadow = activeScene.GetVisibleEntities().shadows[i];
        auto& lightData     = registry.GetComponent<Component::LightData>(visibleShadow);
        auto& shadowData    = lightData.shadow.value();
        const bool isCube   = lightData.GetType() == LIGHT_TYPE_POINT;
        for (auto vI = 0u; vI < visibleShadow.viewports.size(); vI++) {
            auto& viewPort = visibleShadow.viewports.at(vI);
            auto& fb       = shadowData.frameBuffers.at(vI);
            OGLRenderPassInfo info;
            info.name                         = "Shadow_" + std::to_string(i) + "_" + std::to_string(vI);
            info.viewportState.viewport       = fb->info.defaultSize;
            info.viewportState.scissorExtent  = fb->info.defaultSize;
            info.frameBufferState.framebuffer = fb;
            info.frameBufferState.drawBuffers = { GL_COLOR_ATTACHMENT0 };
            info.frameBufferState.clear.colors.resize(1);
            info.frameBufferState.clear.colors[0] = { .index = 0, .color = { std::numeric_limits<float>::max(), std::numeric_limits<float>::max() } };
            info.frameBufferState.clear.depth     = 1.f;
            OGLBindings globalBindings;
            globalBindings.uniformBuffers[UBO_FRAME_INFO] = OGLBufferBindingInfo {
                .buffer = _frameInfoBuffer,
                .offset = 0,
                .size   = _frameInfoBuffer->size
            };
            globalBindings.storageBuffers[SSBO_SHADOW_CAMERA] = OGLBufferBindingInfo {
                .buffer = shadowData.projBuffer,
                .offset = uint32_t(sizeof(GLSL::Camera) * vI),
                .size   = sizeof(GLSL::Camera)
            };
            for (auto& entity : visibleShadow.viewports.at(vI).meshes) {
                auto& rMesh      = registry.GetComponent<Component::Mesh>(entity).at(entity.lod);
                auto& rTransform = registry.GetComponent<Component::Transform>(entity);
                auto rMeshSkin   = registry.HasComponent<Component::MeshSkin>(entity) ? &registry.GetComponent<Component::MeshSkin>(entity) : nullptr;
                for (auto& [rPrimitive, rMaterial] : rMesh) {
                    const bool isMetRough  = rMaterial->type == MATERIAL_TYPE_METALLIC_ROUGHNESS;
                    const bool isSpecGloss = rMaterial->type == MATERIAL_TYPE_SPECULAR_GLOSSINESS;
                    ShaderLibrary::ProgramKeywords keywords(2);
                    if (isMetRough)
                        keywords[0] = { "MATERIAL_TYPE", "MATERIAL_TYPE_METALLIC_ROUGHNESS" };
                    else if (isSpecGloss)
                        keywords[0] = { "MATERIAL_TYPE", "MATERIAL_TYPE_SPECULAR_GLOSSINESS" };
                    keywords[1]  = { "SHADOW_CUBE", isCube ? "1" : "0" };
                    auto& shader = *_shaders["Shadow"][keywords[0].second][keywords[1].second];
                    if (shader == nullptr)
                        shader = a_Renderer.shaderCompiler.CompileProgram("Shadow", keywords);
                    auto& pipeline             = info.pipelines.emplace_back(GetCommonGraphicsPipeline(globalBindings, *rPrimitive, *rMaterial, rTransform, rMeshSkin));
                    auto& gpInfo               = std::get<OGLGraphicsPipelineInfo>(pipeline);
                    gpInfo.shaderState.program = shader;
                }
            }
            // CREATE RENDER PASS
            renderPasses.emplace_back(new OGLRenderPass(info));
        }
    }
}

void PathDfd::_UpdateRenderPassGeometry(Renderer::Impl& a_Renderer)
{
    auto& activeScene   = *a_Renderer.activeScene;
    auto& registry      = *activeScene.GetRegistry();
    auto globalBindings = _GetGlobalBindings();
    auto& info          = _renderPassGeometryInfo;
    // FILL GRAPHICS PIPELINES
    info.pipelines.clear();
    // RENDER SKYBOX IF NEEDED
    if (activeScene.GetSkybox().texture != nullptr) {
        auto skybox  = a_Renderer.LoadTexture(activeScene.GetSkybox().texture.get());
        auto sampler = activeScene.GetSkybox().sampler != nullptr ? a_Renderer.LoadSampler(activeScene.GetSkybox().sampler.get()) : nullptr;
        OGLGraphicsPipelineInfo gpInfo;
        gpInfo.shaderState.program                     = a_Renderer.shaderCompiler.CompileProgram("DeferredSkybox");
        gpInfo.vertexInputState                        = { .vertexCount = 3, .vertexArray = _presentVAO };
        gpInfo.inputAssemblyState                      = { .primitiveTopology = GL_TRIANGLES };
        gpInfo.depthStencilState                       = { .enableDepthTest = false };
        gpInfo.rasterizationState                      = { .cullMode = GL_NONE };
        gpInfo.bindings                                = globalBindings;
        gpInfo.bindings.textures[SAMPLERS_SKYBOX]      = { skybox, sampler };
        gpInfo.depthStencilState.enableStencilTest     = false;
        gpInfo.drawCommands.emplace_back().vertexCount = 3;
        info.pipelines.emplace_back(gpInfo);
    }
    // NOW WE RENDER OPAQUE OBJECTS
    for (auto& mesh : opaqueMeshes) {
        ShaderLibrary::ProgramKeywords keywords(2);
        if (mesh.isMetRough)
            keywords[0] = { "MATERIAL_TYPE", "MATERIAL_TYPE_METALLIC_ROUGHNESS" };
        else if (mesh.isSpecGloss)
            keywords[0] = { "MATERIAL_TYPE", "MATERIAL_TYPE_SPECULAR_GLOSSINESS" };
        keywords[1]  = { "MATERIAL_UNLIT", mesh.isUnlit ? "1" : "0" };
        auto& shader = *_shaders["DeferredGeometry"][keywords[0].second][keywords[1].second];
        if (shader == nullptr)
            shader = a_Renderer.shaderCompiler.CompileProgram("DeferredGeometry", keywords);
        auto& gpInfo                               = std::get<OGLGraphicsPipelineInfo>(info.pipelines.emplace_back(mesh.pipeline));
        gpInfo.depthStencilState.enableStencilTest = true;
        gpInfo.depthStencilState.front.passOp      = GL_REPLACE;
        gpInfo.depthStencilState.front.reference   = 255;
        gpInfo.depthStencilState.back              = gpInfo.depthStencilState.front;
        gpInfo.shaderState.program                 = shader;
    }
    // CREATE RENDER PASS
    renderPasses.emplace_back(new OGLRenderPass(info));
}

void PathDfd::_UpdateRenderPassLight(Renderer::Impl& a_Renderer)
{
    auto& activeScene   = *a_Renderer.activeScene;
    auto globalBindings = _GetGlobalBindings();
    auto& info          = _renderPassLightInfo;
    // FILL GRAPHICS PIPELINES
    info.pipelines.clear();
    // DO VTFS LIGHTING
    {
        auto& shader = *_shaders["DeferredVTFS"];
        if (shader == nullptr)
            shader = a_Renderer.shaderCompiler.CompileProgram("DeferredVTFS");
        auto& gpInfo              = std::get<OGLGraphicsPipelineInfo>(info.pipelines.emplace_back(OGLGraphicsPipelineInfo {}));
        gpInfo.inputAssemblyState = { .primitiveTopology = GL_TRIANGLES };
        gpInfo.rasterizationState = { .cullMode = GL_NONE };
        gpInfo.vertexInputState   = { .vertexCount = 3, .vertexArray = _presentVAO };
        gpInfo.bindings           = globalBindings;
        gpInfo.bindings.images[0] = { _fbGeometry->info.colorBuffers[OUTPUT_FRAG_DFD_GBUFFER0].texture, GL_READ_WRITE, GL_RGBA32UI };
        gpInfo.bindings.images[1] = { _fbGeometry->info.colorBuffers[OUTPUT_FRAG_DFD_GBUFFER1].texture, GL_READ_WRITE, GL_RGBA32UI };
        gpInfo.colorBlend.attachmentStates.resize(1);
        gpInfo.shaderState.program                     = shader;
        gpInfo.depthStencilState.enableDepthTest       = false;
        gpInfo.depthStencilState.enableStencilTest     = true;
        gpInfo.depthStencilState.front.compareOp       = GL_EQUAL;
        gpInfo.depthStencilState.front.reference       = 255;
        gpInfo.depthStencilState.back                  = gpInfo.depthStencilState.front;
        gpInfo.drawCommands.emplace_back().vertexCount = 3;
    }
    // DO IBL LIGHTING
    {
        auto& shader = *_shaders["DeferredIBL"];
        if (shader == nullptr)
            shader = a_Renderer.shaderCompiler.CompileProgram("DeferredIBL");
        auto& gpInfo              = std::get<OGLGraphicsPipelineInfo>(info.pipelines.emplace_back(OGLGraphicsPipelineInfo {}));
        gpInfo.inputAssemblyState = { .primitiveTopology = GL_TRIANGLES };
        gpInfo.rasterizationState = { .cullMode = GL_NONE };
        gpInfo.vertexInputState   = { .vertexCount = 3, .vertexArray = _presentVAO };
        gpInfo.bindings           = globalBindings;
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
        gpInfo.shaderState.program                                = shader;
        gpInfo.depthStencilState.enableDepthTest                  = false;
        gpInfo.depthStencilState.enableStencilTest                = true;
        gpInfo.depthStencilState.front.compareOp                  = GL_EQUAL;
        gpInfo.depthStencilState.front.reference                  = 255;
        gpInfo.depthStencilState.back                             = gpInfo.depthStencilState.front;
        gpInfo.drawCommands.emplace_back().vertexCount            = 3;
    }
    // DO SHADOWS LIGHTING
    {
        const ShaderLibrary::ProgramKeywords keywords = { { "SHADOW_QUALITY", std::to_string(int(a_Renderer.shadowQuality) + 1) } };
        auto& shader                                  = *_shaders["DeferredShadows"][keywords[0].second];
        if (shader == nullptr)
            shader = a_Renderer.shaderCompiler.CompileProgram("DeferredShadows", keywords);
        auto& gpInfo              = std::get<OGLGraphicsPipelineInfo>(info.pipelines.emplace_back(OGLGraphicsPipelineInfo {}));
        gpInfo.inputAssemblyState = { .primitiveTopology = GL_TRIANGLES };
        gpInfo.rasterizationState = { .cullMode = GL_NONE };
        gpInfo.vertexInputState   = { .vertexCount = 3, .vertexArray = _presentVAO };
        gpInfo.bindings           = globalBindings;
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
        gpInfo.drawCommands.emplace_back().vertexCount            = 3;
    }
    // DO SSAO
    {
        const ShaderLibrary::ProgramKeywords keywords = { { "SSAO_QUALITY", std::to_string(int(a_Renderer.shadowQuality) + 1) } };
        auto& shader                                  = *_shaders["DeferredSSAO"][keywords[0].second];
        if (shader == nullptr)
            shader = a_Renderer.shaderCompiler.CompileProgram("DeferredSSAO", keywords);
        auto& gpInfo                                   = std::get<OGLGraphicsPipelineInfo>(info.pipelines.emplace_back(OGLGraphicsPipelineInfo {}));
        gpInfo.inputAssemblyState                      = { .primitiveTopology = GL_TRIANGLES };
        gpInfo.rasterizationState                      = { .cullMode = GL_NONE };
        gpInfo.vertexInputState                        = { .vertexCount = 3, .vertexArray = _presentVAO };
        gpInfo.bindings                                = globalBindings;
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
        gpInfo.drawCommands.emplace_back().vertexCount            = 3;
    }
    // DO FOG
    {
        auto& shader = *_shaders["DeferredFog"];
        if (shader == nullptr)
            shader = a_Renderer.shaderCompiler.CompileProgram("DeferredFog");
        auto& gpInfo              = std::get<OGLGraphicsPipelineInfo>(info.pipelines.emplace_back(OGLGraphicsPipelineInfo {}));
        gpInfo.inputAssemblyState = { .primitiveTopology = GL_TRIANGLES };
        gpInfo.rasterizationState = { .cullMode = GL_NONE };
        gpInfo.vertexInputState   = { .vertexCount = 3, .vertexArray = _presentVAO };
        gpInfo.bindings           = globalBindings;
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
        gpInfo.drawCommands.emplace_back().vertexCount            = 3;
    }
    // CREATE RENDER PASS
    renderPasses.emplace_back(new OGLRenderPass(info));
}

void PathDfd::_UpdateRenderPassOIT(Renderer::Impl& a_Renderer)
{
    auto& activeScene   = *a_Renderer.activeScene;
    auto& registry      = *activeScene.GetRegistry();
    auto globalBindings = _GetGlobalBindings();
    auto shadowQuality  = std::to_string(int(a_Renderer.shadowQuality) + 1);

    {
        uint32_t clearColor = std::numeric_limits<uint32_t>::max();
        _OITDepth->Clear(GL_RED_INTEGER, GL_UNSIGNED_INT, &clearColor);
    }
    if (blendedMeshes.empty())
        return;

    auto& info = _renderPassOITInfo;
    _renderPassOITInfo.pipelines.clear();
    // RENDER DEPTH
    for (auto& mesh : blendedMeshes) {
        ShaderLibrary::ProgramKeywords keywords(1);
        if (mesh.isMetRough)
            keywords[0] = { "MATERIAL_TYPE", "MATERIAL_TYPE_METALLIC_ROUGHNESS" };
        else if (mesh.isSpecGloss)
            keywords[0] = { "MATERIAL_TYPE", "MATERIAL_TYPE_SPECULAR_GLOSSINESS" };
        auto& shader = *_shaders["OITDepth"][keywords[0].second];
        if (shader == nullptr)
            shader = a_Renderer.shaderCompiler.CompileProgram("OITDepth", keywords);
        auto& gpInfo                              = std::get<OGLGraphicsPipelineInfo>(info.pipelines.emplace_back(mesh.pipeline));
        gpInfo.shaderState.program                = shader;
        gpInfo.colorBlend                         = { .attachmentStates = { GetOITBlending() } };
        gpInfo.depthStencilState.enableDepthWrite = false;
        gpInfo.bindings.images[IMG_OIT_DEPTH]     = { .texture = _OITDepth, .access = GL_READ_WRITE, .format = GL_R32UI, .layered = true };
    }
    // RENDER SURFACES
    for (auto& mesh : blendedMeshes) {
        ShaderLibrary::ProgramKeywords keywords(3);
        if (mesh.isMetRough)
            keywords[0] = { "MATERIAL_TYPE", "MATERIAL_TYPE_METALLIC_ROUGHNESS" };
        else if (mesh.isSpecGloss)
            keywords[0] = { "MATERIAL_TYPE", "MATERIAL_TYPE_SPECULAR_GLOSSINESS" };
        keywords[1]  = { "MATERIAL_UNLIT", mesh.isUnlit ? "1" : "0" };
        keywords[2]  = { "SHADOW_QUALITY", shadowQuality };
        auto& shader = *_shaders["OITForward"][keywords[0].second][keywords[1].second][keywords[2].second];
        if (shader == nullptr)
            shader = a_Renderer.shaderCompiler.CompileProgram("OITForward", keywords);
        auto& gpInfo                              = std::get<OGLGraphicsPipelineInfo>(info.pipelines.emplace_back(mesh.pipeline));
        gpInfo.shaderState.program                = shader;
        gpInfo.colorBlend                         = { .attachmentStates = { GetOITBlending() } };
        gpInfo.depthStencilState.enableDepthWrite = false;
        gpInfo.bindings.images[IMG_OIT_COLORS]    = { .texture = _OITColors, .access = GL_WRITE_ONLY, .format = GL_RGBA16F, .layered = true };
        gpInfo.bindings.images[IMG_OIT_DEPTH]     = { .texture = _OITDepth, .access = GL_READ_ONLY, .format = GL_R32F, .layered = true };
    }
    // CREATE RENDER PASS
    renderPasses.emplace_back(new OGLRenderPass(info));
    // CREATE COMPOSITING RENDER PASS
    renderPasses.emplace_back(new OGLRenderPass(_renderPassOITCompositingInfo));
}

void PathDfd::_UpdateRenderPassTemporalAccumulation(Renderer::Impl& a_Renderer)
{
    auto& fbTemporalAccumulation          = _fbTemporalAccumulation[(a_Renderer.frameIndex + 0) % 2];
    auto& fbTemporalAccumulation_Previous = _fbTemporalAccumulation[(a_Renderer.frameIndex + 1) % 2];
    auto& info                            = _renderPassTemporalAccumulationInfo;
    // FILL VIEWPORT STATES
    info.frameBufferState.framebuffer = fbTemporalAccumulation;
    // FILL GRAPHICS PIPELINES
    info.pipelines.clear();
    auto color_Previous                            = fbTemporalAccumulation_Previous != nullptr ? fbTemporalAccumulation_Previous->info.colorBuffers[0].texture : _fbGeometry->info.colorBuffers[OUTPUT_FRAG_DFD_FINAL].texture;
    auto& gpInfo                                   = std::get<OGLGraphicsPipelineInfo>(info.pipelines.emplace_back());
    gpInfo.depthStencilState                       = { .enableDepthTest = false };
    gpInfo.shaderState                             = _shaderTemporalAccumulation;
    gpInfo.inputAssemblyState                      = { .primitiveTopology = GL_TRIANGLES };
    gpInfo.rasterizationState                      = { .cullMode = GL_NONE };
    gpInfo.vertexInputState                        = { .vertexCount = 3, .vertexArray = _presentVAO };
    gpInfo.bindings.textures[0]                    = { .texture = color_Previous, .sampler = _TAASampler };
    gpInfo.bindings.textures[1]                    = { .texture = _fbGeometry->info.colorBuffers[OUTPUT_FRAG_DFD_FINAL].texture, .sampler = _TAASampler };
    gpInfo.bindings.textures[2]                    = { .texture = _fbGeometry->info.colorBuffers[OUTPUT_FRAG_DFD_VELOCITY].texture, .sampler = _TAASampler };
    gpInfo.drawCommands.emplace_back().vertexCount = 3;
    // CREATE RENDER PASS
    renderPasses.emplace_back(new OGLRenderPass(info));
}

void PathDfd::_UpdateRenderPassPresent(Renderer::Impl& a_Renderer)
{
    auto& renderBuffer           = *a_Renderer.activeRenderBuffer;
    auto& fbTemporalAccumulation = _fbTemporalAccumulation[a_Renderer.frameIndex % 2];
    auto& info                   = _renderPassPresentInfo;
    // FILL GRAPHICS PIPELINES
    info.pipelines.clear();
    auto& gpInfo                                   = std::get<OGLGraphicsPipelineInfo>(info.pipelines.emplace_back());
    gpInfo.depthStencilState                       = { .enableDepthTest = false };
    gpInfo.shaderState                             = _shaderPresent;
    gpInfo.inputAssemblyState                      = { .primitiveTopology = GL_TRIANGLES };
    gpInfo.rasterizationState                      = { .cullMode = GL_NONE };
    gpInfo.vertexInputState                        = { .vertexCount = 3, .vertexArray = _presentVAO };
    gpInfo.bindings.images[0]                      = { fbTemporalAccumulation->info.colorBuffers[0].texture, GL_READ_ONLY, GL_RGBA16F };
    gpInfo.bindings.images[1]                      = { renderBuffer, GL_WRITE_ONLY, GL_RGBA8 };
    gpInfo.drawCommands.emplace_back().vertexCount = 3;
    // CREATE RENDER PASS
    renderPasses.emplace_back(new OGLRenderPass(info));
}
}
