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
#include <MSG/Renderer/OGL/RendererPathFwd.hpp>
#include <MSG/Scene.hpp>
#include <MSG/Texture.hpp>
#include <MSG/Tools/Halton.hpp>
#include <MSG/Transform.hpp>

#include <Material.glsl>

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

auto CreatePresentVAO(OGLContext& a_Context)
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

/**
 * Forward Render Target :
 * RT0 : BRDF CDiff/BRDF Alpha (R), BRDF F0/AO (G) GL_RG32UI
 * RT2 : World Normal (RGB)                        GL_RGB16_SNORM
 * RT3 : Velocity (RG)                             GL_RG16F
 * RT4 : Color (Unlit/Emissive/Final Color)        GL_RGBA16F
 * Depth                                           GL_DEPTH_COMPONENT24
 */
auto CreateFbOpaque(
    OGLContext& a_Context,
    const glm::uvec2& a_Size)
{
    OGLFrameBufferCreateInfo info;
    info.defaultSize = { a_Size, 1 };
    info.colorBuffers.resize(OUTPUT_FRAG_FWD_OPAQUE_COUNT);
    info.colorBuffers[OUTPUT_FRAG_FWD_OPAQUE_COLOR].attachment    = GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_FWD_OPAQUE_COLOR;
    info.colorBuffers[OUTPUT_FRAG_FWD_OPAQUE_VELOCITY].attachment = GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_FWD_OPAQUE_VELOCITY;
    info.colorBuffers[OUTPUT_FRAG_FWD_OPAQUE_COLOR].texture       = std::make_shared<OGLTexture2D>(a_Context, OGLTexture2DInfo { .width = a_Size.x, .height = a_Size.y, .levels = 1, .sizedFormat = GL_RGBA16F });
    info.colorBuffers[OUTPUT_FRAG_FWD_OPAQUE_VELOCITY].texture    = std::make_shared<OGLTexture2D>(a_Context, OGLTexture2DInfo { .width = a_Size.x, .height = a_Size.y, .levels = 1, .sizedFormat = GL_RG16F });
    info.depthBuffer.texture                                      = std::make_shared<OGLTexture2D>(a_Context, OGLTexture2DInfo { .width = a_Size.x, .height = a_Size.y, .levels = 1, .sizedFormat = GL_DEPTH_COMPONENT24 });
    return std::make_shared<OGLFrameBuffer>(a_Context, info);
}

/**
 * Transparent Render Target :
 * RT0 : Accum     GL_RGBA16F
 * RT2 : Revealage GL_R8
 * RT3 : Color     GL_RGB8
 */
auto CreateFbBlended(
    OGLContext& a_Context,
    const glm::uvec2& a_Size,
    const std::shared_ptr<OGLTexture>& a_OpaqueColor,
    const std::shared_ptr<OGLTexture>& a_OpaqueDepth)
{
    OGLFrameBufferCreateInfo info;
    info.defaultSize = { a_Size, 1 };
    info.colorBuffers.resize(OUTPUT_FRAG_FWD_BLENDED_COUNT);
    info.colorBuffers[OUTPUT_FRAG_FWD_BLENDED_ACCUM].attachment = GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_FWD_BLENDED_ACCUM;
    info.colorBuffers[OUTPUT_FRAG_FWD_BLENDED_REV].attachment   = GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_FWD_BLENDED_REV;
    info.colorBuffers[OUTPUT_FRAG_FWD_BLENDED_COLOR].attachment = GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_FWD_BLENDED_COLOR;
    info.colorBuffers[OUTPUT_FRAG_FWD_BLENDED_ACCUM].texture    = std::make_shared<OGLTexture2D>(a_Context, OGLTexture2DInfo { .width = a_Size.x, .height = a_Size.y, .levels = 1, .sizedFormat = GL_RGBA16F });
    info.colorBuffers[OUTPUT_FRAG_FWD_BLENDED_REV].texture      = std::make_shared<OGLTexture2D>(a_Context, OGLTexture2DInfo { .width = a_Size.x, .height = a_Size.y, .levels = 1, .sizedFormat = GL_R8 });
    info.colorBuffers[OUTPUT_FRAG_FWD_BLENDED_COLOR].texture    = a_OpaqueColor;
    info.depthBuffer.texture                                    = a_OpaqueDepth;
    return std::make_shared<OGLFrameBuffer>(a_Context, info);
}

auto CreateFbCompositing(
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

auto CreateFbTemporalAccumulation(
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

auto CreateFbPresent(
    OGLContext& a_Context,
    const glm::uvec2& a_Size)
{
    OGLFrameBufferCreateInfo info;
    info.defaultSize = { a_Size, 1 };
    return std::make_shared<OGLFrameBuffer>(a_Context, info);
}

constexpr std::array<OGLColorBlendAttachmentState, 3> GetBlendedOGLColorBlendAttachmentState()
{
    constexpr OGLColorBlendAttachmentState blendAccum {
        .index               = OUTPUT_FRAG_FWD_BLENDED_ACCUM,
        .enableBlend         = true,
        .srcColorBlendFactor = GL_ONE,
        .dstColorBlendFactor = GL_ONE,
        .srcAlphaBlendFactor = GL_ONE,
        .dstAlphaBlendFactor = GL_ONE
    };
    constexpr OGLColorBlendAttachmentState blendRev {
        .index               = OUTPUT_FRAG_FWD_BLENDED_REV,
        .enableBlend         = true,
        .srcColorBlendFactor = GL_ZERO,
        .dstColorBlendFactor = GL_ONE_MINUS_SRC_COLOR,
        .srcAlphaBlendFactor = GL_ZERO,
        .dstAlphaBlendFactor = GL_ONE_MINUS_SRC_COLOR
    };
    constexpr OGLColorBlendAttachmentState blendColor {
        .index               = OUTPUT_FRAG_FWD_BLENDED_COLOR,
        .enableBlend         = true,
        .srcColorBlendFactor = GL_ZERO,
        .dstColorBlendFactor = GL_ONE_MINUS_SRC_COLOR,
        .srcAlphaBlendFactor = GL_ZERO,
        .dstAlphaBlendFactor = GL_ONE_MINUS_SRC_COLOR
    };
    return { blendAccum, blendRev, blendColor };
}

auto GetCommonGraphicsPipeline(
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

auto GetSkyboxGraphicsPipeline(
    Renderer::Impl& a_Renderer,
    const std::shared_ptr<OGLVertexArray>& a_PresentVAO,
    const OGLBindings& a_GlobalBindings,
    const std::shared_ptr<OGLTexture>& a_Skybox,
    const std::shared_ptr<OGLSampler>& a_Sampler)
{
    OGLGraphicsPipelineInfo info;
    info.shaderState.program                = a_Renderer.shaderCompiler.CompileProgram("Skybox");
    info.vertexInputState                   = { .vertexCount = 3, .vertexArray = a_PresentVAO };
    info.inputAssemblyState                 = { .primitiveTopology = GL_TRIANGLES };
    info.depthStencilState                  = { .enableDepthTest = false };
    info.rasterizationState                 = { .cullMode = GL_NONE };
    info.bindings                           = a_GlobalBindings;
    info.bindings.textures[SAMPLERS_SKYBOX] = { a_Skybox, a_Sampler };
    return info;
}

auto GetStandardBRDF()
{
    static auto brdfLutTexture = new Texture(BRDF::GenerateTexture(BRDF::Type::Standard));
    return brdfLutTexture;
}

PathFwd::PathFwd(Renderer::Impl& a_Renderer, const RendererSettings& a_Settings)
    : _fogCuller(a_Renderer)
    , _lightCuller(a_Renderer)
    , _frameInfoBuffer(std::make_shared<OGLTypedBuffer<GLSL::FrameInfo>>(a_Renderer.context))
    , _cameraBuffer(std::make_shared<OGLTypedBuffer<GLSL::CameraUBO>>(a_Renderer.context))
    , _fogSampler(std::make_shared<OGLSampler>(a_Renderer.context, OGLSamplerParameters { .minFilter = GL_LINEAR, .magFilter = GL_LINEAR, .wrapS = GL_CLAMP_TO_EDGE, .wrapT = GL_CLAMP_TO_EDGE, .wrapR = GL_CLAMP_TO_EDGE }))
    , _shadowSampler(std::make_shared<OGLSampler>(a_Renderer.context, OGLSamplerParameters { .wrapS = GL_CLAMP_TO_BORDER, .wrapT = GL_CLAMP_TO_BORDER, .wrapR = GL_CLAMP_TO_BORDER, .compareMode = GL_COMPARE_REF_TO_TEXTURE, .compareFunc = GL_LEQUAL, .borderColor = { 1, 1, 1, 1 } }))
    , _TAASampler(std::make_shared<OGLSampler>(a_Renderer.context, OGLSamplerParameters { .wrapS = GL_CLAMP_TO_EDGE, .wrapT = GL_CLAMP_TO_EDGE, .wrapR = GL_CLAMP_TO_EDGE }))
    , _iblSpecSampler(std::make_shared<OGLSampler>(a_Renderer.context, OGLSamplerParameters { .minFilter = GL_LINEAR_MIPMAP_LINEAR }))
    , _brdfLutSampler(std::make_shared<OGLSampler>(a_Renderer.context, OGLSamplerParameters { .wrapS = GL_CLAMP_TO_EDGE, .wrapT = GL_CLAMP_TO_EDGE, .wrapR = GL_CLAMP_TO_EDGE }))
    , _brdfLut(a_Renderer.LoadTexture(GetStandardBRDF()))
    , _shaderFogRendering({ .program = a_Renderer.shaderCompiler.CompileProgram("FogRendering") })
    , _shaderShadowMetRough({ .program = a_Renderer.shaderCompiler.CompileProgram("Shadow_MetRough") })
    , _shaderShadowSpecGloss({ .program = a_Renderer.shaderCompiler.CompileProgram("Shadow_SpecGloss") })
    , _shaderShadowMetRoughCube({ .program = a_Renderer.shaderCompiler.CompileProgram("Shadow_MetRough_Cube") })
    , _shaderShadowSpecGlossCube({ .program = a_Renderer.shaderCompiler.CompileProgram("Shadow_SpecGloss_Cube") })
    , _shaderMetRoughOpaque({ .program = a_Renderer.shaderCompiler.CompileProgram("FwdMetRough_Opaque") })
    , _shaderSpecGlossOpaque({ .program = a_Renderer.shaderCompiler.CompileProgram("FwdSpecGloss_Opaque") })
    , _shaderMetRoughBlended({ .program = a_Renderer.shaderCompiler.CompileProgram("FwdMetRough_Blended") })
    , _shaderSpecGlossBlended({ .program = a_Renderer.shaderCompiler.CompileProgram("FwdSpecGloss_Blended") })
    , _shaderMetRoughOpaqueUnlit({ .program = a_Renderer.shaderCompiler.CompileProgram("FwdMetRough_Opaque_Unlit") })
    , _shaderSpecGlossOpaqueUnlit({ .program = a_Renderer.shaderCompiler.CompileProgram("FwdSpecGloss_Opaque_Unlit") })
    , _shaderMetRoughBlendedUnlit({ .program = a_Renderer.shaderCompiler.CompileProgram("FwdMetRough_Blended_Unlit") })
    , _shaderSpecGlossBlendedUnlit({ .program = a_Renderer.shaderCompiler.CompileProgram("FwdSpecGloss_Blended_Unlit") })
    , _shaderCompositing({ .program = a_Renderer.shaderCompiler.CompileProgram("Compositing") })
    , _shaderTemporalAccumulation({ .program = a_Renderer.shaderCompiler.CompileProgram("TemporalAccumulation") })
    , _shaderPresent({ .program = a_Renderer.shaderCompiler.CompileProgram("Present") })
    , _presentVAO(CreatePresentVAO(a_Renderer.context))
{
}

void PathFwd::Update(Renderer::Impl& a_Renderer)
{
    renderPasses.clear();
    _UpdateFrameInfo(a_Renderer);
    _UpdateCamera(a_Renderer);
    _UpdateLights(a_Renderer);
    _UpdateRenderPassShadows(a_Renderer);
    _UpdateRenderPassOpaque(a_Renderer);
    _UpdateRenderPassBlended(a_Renderer);
    _UpdateRenderPassCompositing(a_Renderer);
    _UpdateRenderPassFog(a_Renderer);
    _UpdateRenderPassTemporalAccumulation(a_Renderer);
    _UpdateRenderPassPresent(a_Renderer);
}

OGLBindings PathFwd::_GetGlobalBindings() const
{
    OGLBindings bindings;
    bindings.uniformBuffers[UBO_FRAME_INFO]     = { _frameInfoBuffer, 0, _frameInfoBuffer->size };
    bindings.uniformBuffers[UBO_CAMERA]         = { _cameraBuffer, 0, _cameraBuffer->size };
    bindings.uniformBuffers[UBO_FWD_IBL]        = { _lightCuller.ibls.buffer, 0, _lightCuller.ibls.buffer->size };
    bindings.uniformBuffers[UBO_FWD_SHADOWS]    = { _lightCuller.shadows.buffer, 0, _lightCuller.shadows.buffer->size };
    bindings.storageBuffers[SSBO_VTFS_LIGHTS]   = { _lightCuller.vtfs.buffer.lightsBuffer, offsetof(GLSL::VTFSLightsBuffer, lights), _lightCuller.vtfs.buffer.lightsBuffer->size };
    bindings.storageBuffers[SSBO_VTFS_CLUSTERS] = { _lightCuller.vtfs.buffer.cluster, 0, _lightCuller.vtfs.buffer.cluster->size };
    bindings.textures[SAMPLERS_BRDF_LUT]        = { _brdfLut, _brdfLutSampler };
    for (auto i = 0u; i < _lightCuller.ibls.buffer->Get().count; i++) {
        auto& texture                           = _lightCuller.ibls.textures.at(i);
        bindings.textures[SAMPLERS_FWD_IBL + i] = { .texture = texture, .sampler = _iblSpecSampler };
    }
    for (auto i = 0u; i < _lightCuller.shadows.buffer->Get().count; i++) {
        auto& texture                              = _lightCuller.shadows.textures.at(i);
        bindings.textures[SAMPLERS_FWD_SHADOW + i] = { .texture = texture, .sampler = _shadowSampler };
    }
    return bindings;
}

void PathFwd::_UpdateFrameInfo(Renderer::Impl& a_Renderer)
{
    GLSL::FrameInfo frameInfo;
    frameInfo.width      = (*a_Renderer.activeRenderBuffer)->width;
    frameInfo.height     = (*a_Renderer.activeRenderBuffer)->height;
    frameInfo.frameIndex = a_Renderer.frameIndex;
    _frameInfoBuffer->Set(frameInfo);
    _frameInfoBuffer->Update();
}

void PathFwd::_UpdateCamera(Renderer::Impl& a_Renderer)
{
    auto& activeScene                = *a_Renderer.activeScene;
    auto& currentCamera              = activeScene.GetCamera();
    GLSL::CameraUBO cameraUBOData    = _cameraBuffer->Get();
    cameraUBOData.previous           = cameraUBOData.current;
    cameraUBOData.current.position   = currentCamera.GetComponent<MSG::Transform>().GetWorldPosition();
    cameraUBOData.current.projection = currentCamera.GetComponent<Camera>().projection.GetMatrix();
    if (a_Renderer.enableTAA)
        cameraUBOData.current.projection = ApplyTemporalJitter(cameraUBOData.current.projection, uint8_t(a_Renderer.frameIndex));
    cameraUBOData.current.view = glm::inverse(currentCamera.GetComponent<MSG::Transform>().GetWorldTransformMatrix());
    _cameraBuffer->Set(cameraUBOData);
    _cameraBuffer->Update();
}

void PathFwd::_UpdateLights(Renderer::Impl& a_Renderer)
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

void PathFwd::_UpdateRenderPassShadows(Renderer::Impl& a_Renderer)
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
            info.frameBufferState.clear.depth = 1.f;
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
                    auto& pipelineInfo     = info.pipelines.emplace_back(GetCommonGraphicsPipeline(globalBindings, *rPrimitive, *rMaterial, rTransform, rMeshSkin));
                    if (isMetRough)
                        std::get<OGLGraphicsPipelineInfo>(pipelineInfo).shaderState = isCube ? _shaderShadowMetRoughCube : _shaderShadowMetRough;
                    else if (isSpecGloss)
                        std::get<OGLGraphicsPipelineInfo>(pipelineInfo).shaderState = isCube ? _shaderShadowSpecGlossCube : _shaderShadowSpecGloss;
                }
            }
            // CREATE RENDER PASS
            renderPasses.emplace_back(new OGLRenderPass(info));
        }
    }
}

void PathFwd::_UpdateRenderPassOpaque(Renderer::Impl& a_Renderer)
{
    auto& activeScene     = *a_Renderer.activeScene;
    auto& registry        = *activeScene.GetRegistry();
    auto& renderBuffer    = *a_Renderer.activeRenderBuffer;
    auto renderBufferSize = glm::uvec3(renderBuffer->width, renderBuffer->height, 1);
    auto fbOpaqueSize     = _fbOpaque != nullptr ? _fbOpaque->info.defaultSize : glm::uvec3(0);
    auto& clearColor      = activeScene.GetBackgroundColor();
    auto globalBindings   = _GetGlobalBindings();
    auto& info            = _renderPassOpaqueInfo;
    if (fbOpaqueSize != renderBufferSize) {
        _fbOpaque = CreateFbOpaque(a_Renderer.context, renderBufferSize);
        // FILL VIEWPORT STATES
        info.name                         = "FwdOpaque";
        info.viewportState.viewport       = { renderBuffer->width, renderBuffer->height };
        info.viewportState.scissorExtent  = { renderBuffer->width, renderBuffer->height };
        info.frameBufferState.framebuffer = _fbOpaque;
        info.frameBufferState.clear.colors.resize(OUTPUT_FRAG_FWD_OPAQUE_COUNT);
        info.frameBufferState.clear.colors[OUTPUT_FRAG_FWD_OPAQUE_COLOR]    = { OUTPUT_FRAG_FWD_OPAQUE_COLOR, { clearColor.r, clearColor.g, clearColor.b } };
        info.frameBufferState.clear.colors[OUTPUT_FRAG_FWD_OPAQUE_VELOCITY] = { OUTPUT_FRAG_FWD_OPAQUE_VELOCITY, { 0.f, 0.f } };
        info.frameBufferState.clear.depth                                   = 1.f;
        info.frameBufferState.drawBuffers                                   = {
            GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_FWD_OPAQUE_COLOR,
            GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_FWD_OPAQUE_VELOCITY
        };
    }
    // FILL GRAPHICS PIPELINES
    info.pipelines.clear();
    if (activeScene.GetSkybox().texture != nullptr) { // first render Skybox if needed
        auto skybox  = a_Renderer.LoadTexture(activeScene.GetSkybox().texture.get());
        auto sampler = activeScene.GetSkybox().sampler != nullptr ? a_Renderer.LoadSampler(activeScene.GetSkybox().sampler.get()) : nullptr;
        info.pipelines.emplace_back(GetSkyboxGraphicsPipeline(a_Renderer, _presentVAO, globalBindings, skybox, sampler));
        std::get<OGLGraphicsPipelineInfo>(info.pipelines.front()).drawCommands.emplace_back().vertexCount = 3;
    }
    for (auto& entity : activeScene.GetVisibleEntities().meshes) {
        auto& rMesh      = registry.GetComponent<Component::Mesh>(entity).at(entity.lod);
        auto& rTransform = registry.GetComponent<Component::Transform>(entity);
        auto rMeshSkin   = registry.HasComponent<Component::MeshSkin>(entity) ? &registry.GetComponent<Component::MeshSkin>(entity) : nullptr;
        for (auto& [rPrimitive, rMaterial] : rMesh) {
            const bool isAlphaBlend = rMaterial->alphaMode == MATERIAL_ALPHA_BLEND;
            const bool isMetRough   = rMaterial->type == MATERIAL_TYPE_METALLIC_ROUGHNESS;
            const bool isSpecGloss  = rMaterial->type == MATERIAL_TYPE_SPECULAR_GLOSSINESS;
            const bool isUnlit      = rMaterial->unlit;
            // is there any chance we have opaque pixels here ?
            // it's ok to use specularGlossiness.diffuseFactor even with metrough because they share type/location
            if (isAlphaBlend && rMaterial->buffer->Get().specularGlossiness.diffuseFactor.a < 1)
                continue;
            auto& pipeline = info.pipelines.emplace_back(GetCommonGraphicsPipeline(globalBindings, *rPrimitive, *rMaterial, rTransform, rMeshSkin));
            if (isMetRough)
                std::get<OGLGraphicsPipelineInfo>(pipeline).shaderState = isUnlit ? _shaderMetRoughOpaqueUnlit : _shaderMetRoughOpaque;
            else if (isSpecGloss)
                std::get<OGLGraphicsPipelineInfo>(pipeline).shaderState = isUnlit ? _shaderSpecGlossOpaqueUnlit : _shaderSpecGlossOpaque;
        }
    }
    // CREATE RENDER PASS
    renderPasses.emplace_back(new OGLRenderPass(info));
}

void PathFwd::_UpdateRenderPassBlended(Renderer::Impl& a_Renderer)
{
    constexpr auto colorBlendStates = GetBlendedOGLColorBlendAttachmentState();
    auto& activeScene               = *a_Renderer.activeScene;
    auto& registry                  = *activeScene.GetRegistry();
    auto& fbOpaque                  = _fbOpaque;
    auto fbOpaqueSize               = fbOpaque->info.defaultSize;
    auto fbBlendSize                = _fbBlended != nullptr ? _fbBlended->info.defaultSize : glm::uvec3(0);
    auto globalBindings             = _GetGlobalBindings();
    auto& info                      = _renderPassBlendedInfo;
    if (fbOpaqueSize != fbBlendSize) {
        _fbBlended = CreateFbBlended(
            a_Renderer.context, fbOpaqueSize,
            fbOpaque->info.colorBuffers[OUTPUT_FRAG_FWD_OPAQUE_COLOR].texture,
            fbOpaque->info.depthBuffer.texture);
        // FILL VIEWPORT STATES
        info.name                         = "FwdBlended";
        info.viewportState                = _renderPassOpaqueInfo.viewportState;
        info.frameBufferState.framebuffer = _fbBlended;
        info.frameBufferState.clear.colors.resize(OUTPUT_FRAG_FWD_BLENDED_COUNT);
        info.frameBufferState.clear.colors[OUTPUT_FRAG_FWD_BLENDED_ACCUM] = { OUTPUT_FRAG_FWD_BLENDED_ACCUM, { 0.f, 0.f, 0.f, 0.f } };
        info.frameBufferState.clear.colors[OUTPUT_FRAG_FWD_BLENDED_REV]   = { OUTPUT_FRAG_FWD_BLENDED_REV, { 1.f } };
        info.frameBufferState.drawBuffers                                 = {
            GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_FWD_BLENDED_ACCUM,
            GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_FWD_BLENDED_REV,
            GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_FWD_BLENDED_COLOR
        };
    }
    // FILL GRAPHICS PIPELINES
    info.pipelines.clear();
    for (auto& entity : activeScene.GetVisibleEntities().meshes | std::views::reverse) {
        auto& rMesh      = registry.GetComponent<Component::Mesh>(entity).at(entity.lod);
        auto& rTransform = registry.GetComponent<Component::Transform>(entity);
        auto rMeshSkin   = registry.HasComponent<Component::MeshSkin>(entity) ? &registry.GetComponent<Component::MeshSkin>(entity) : nullptr;
        for (auto& [rPrimitive, rMaterial] : rMesh) {
            const bool isAlphaBlend = rMaterial->alphaMode == MATERIAL_ALPHA_BLEND;
            const bool isMetRough   = rMaterial->type == MATERIAL_TYPE_METALLIC_ROUGHNESS;
            const bool isSpecGloss  = rMaterial->type == MATERIAL_TYPE_SPECULAR_GLOSSINESS;
            const bool isUnlit      = rMaterial->unlit;
            if (!isAlphaBlend)
                continue;
            auto& pipeline                            = info.pipelines.emplace_back(GetCommonGraphicsPipeline(globalBindings, *rPrimitive, *rMaterial, rTransform, rMeshSkin));
            auto& gpInfo                              = std::get<OGLGraphicsPipelineInfo>(pipeline);
            gpInfo.colorBlend.attachmentStates        = { colorBlendStates.begin(), colorBlendStates.end() };
            gpInfo.depthStencilState.enableDepthWrite = false;
            if (isMetRough)
                gpInfo.shaderState = isUnlit ? _shaderMetRoughBlendedUnlit : _shaderMetRoughBlended;
            else if (isSpecGloss)
                gpInfo.shaderState = isUnlit ? _shaderSpecGlossBlendedUnlit : _shaderSpecGlossBlended;
        }
    }
    // CREATE RENDER PASS
    renderPasses.emplace_back(new OGLRenderPass(info));
}

void PathFwd::_UpdateRenderPassCompositing(Renderer::Impl& a_Renderer)
{
    auto fbBlendSize       = _fbBlended->info.defaultSize;
    auto fbCompositingSize = _fbCompositing != nullptr ? _fbCompositing->info.defaultSize : glm::uvec3(0);
    auto& info             = _renderPassCompositingInfo;
    if (fbBlendSize != fbCompositingSize) {
        _fbCompositing = CreateFbCompositing(
            a_Renderer.context, fbBlendSize,
            _fbBlended->info.colorBuffers[OUTPUT_FRAG_FWD_BLENDED_COLOR].texture);
        constexpr OGLColorBlendAttachmentState blending {
            .index               = OUTPUT_FRAG_FWD_COMP_COLOR,
            .enableBlend         = true,
            .srcColorBlendFactor = GL_ONE_MINUS_SRC_ALPHA,
            .dstColorBlendFactor = GL_ONE,
            .srcAlphaBlendFactor = GL_ONE_MINUS_SRC_ALPHA,
            .dstAlphaBlendFactor = GL_ONE
        };
        // FILL VIEWPORT STATES
        info.name                         = "Compositing";
        info.viewportState                = _renderPassBlendedInfo.viewportState;
        info.frameBufferState             = { .framebuffer = _fbCompositing };
        info.frameBufferState.drawBuffers = { GL_COLOR_ATTACHMENT0 };
        // FILL GRAPHICS PIPELINES
        info.pipelines.clear();
        auto& gpInfo                                   = std::get<OGLGraphicsPipelineInfo>(info.pipelines.emplace_back());
        gpInfo.colorBlend                              = { .attachmentStates = { blending } };
        gpInfo.depthStencilState                       = { .enableDepthTest = false };
        gpInfo.shaderState                             = _shaderCompositing;
        gpInfo.inputAssemblyState                      = { .primitiveTopology = GL_TRIANGLES };
        gpInfo.rasterizationState                      = { .cullMode = GL_NONE };
        gpInfo.vertexInputState                        = { .vertexCount = 3, .vertexArray = _presentVAO };
        gpInfo.bindings.images[0]                      = { _fbBlended->info.colorBuffers[OUTPUT_FRAG_FWD_BLENDED_ACCUM].texture, GL_READ_ONLY, GL_RGBA16F };
        gpInfo.bindings.images[1]                      = { _fbBlended->info.colorBuffers[OUTPUT_FRAG_FWD_BLENDED_REV].texture, GL_READ_ONLY, GL_R8 };
        gpInfo.drawCommands.emplace_back().vertexCount = 3;
    }
    // CREATE RENDER PASS
    renderPasses.emplace_back(new OGLRenderPass(info));
}

void PathFwd::_UpdateRenderPassFog(Renderer::Impl& a_Renderer)
{
    auto updatePass = _fogCuller.Update(*a_Renderer.activeScene,
        _lightCuller, _shadowSampler, _cameraBuffer, _frameInfoBuffer);
    if (updatePass == nullptr)
        return; // no fog, no need to continue
    renderPasses.emplace_back(updatePass);
    auto& info            = _renderPassFogInfo;
    info.name             = "FogRendering";
    info.viewportState    = _renderPassCompositingInfo.viewportState;
    info.frameBufferState = _renderPassCompositingInfo.frameBufferState;
    constexpr OGLColorBlendAttachmentState blending {
        .index               = 0,
        .enableBlend         = true,
        .srcColorBlendFactor = GL_SRC_ALPHA,
        .dstColorBlendFactor = GL_ONE_MINUS_SRC_ALPHA,
        .srcAlphaBlendFactor = GL_ZERO,
        .dstAlphaBlendFactor = GL_ONE
    };
    info.pipelines.clear();
    auto& gpInfo                                   = std::get<OGLGraphicsPipelineInfo>(info.pipelines.emplace_back());
    gpInfo.colorBlend                              = { .attachmentStates = { blending } };
    gpInfo.depthStencilState                       = { .enableDepthTest = false };
    gpInfo.shaderState                             = _shaderFogRendering;
    gpInfo.inputAssemblyState                      = { .primitiveTopology = GL_TRIANGLES };
    gpInfo.rasterizationState                      = { .cullMode = GL_NONE };
    gpInfo.vertexInputState                        = { .vertexCount = 3, .vertexArray = _presentVAO };
    gpInfo.bindings.textures[0]                    = { _fogCuller.resultTexture, _fogSampler };
    gpInfo.bindings.textures[1]                    = { _fogCuller.noiseTexture, _fogCuller.noiseSampler };
    gpInfo.bindings.textures[2]                    = { _fbOpaque->info.depthBuffer.texture, _fogSampler };
    gpInfo.bindings.uniformBuffers[UBO_FRAME_INFO] = { _frameInfoBuffer, 0, _frameInfoBuffer->size };
    gpInfo.bindings.uniformBuffers[UBO_CAMERA]     = { _cameraBuffer, 0, _cameraBuffer->size };
    gpInfo.drawCommands.emplace_back().vertexCount = 3;
    // CREATE RENDER PASS
    renderPasses.emplace_back(new OGLRenderPass(info));
}

void PathFwd::_UpdateRenderPassTemporalAccumulation(Renderer::Impl& a_Renderer)
{
    auto& fbTemporalAccumulation          = _fbTemporalAccumulation[(a_Renderer.frameIndex + 0) % 2];
    auto& fbTemporalAccumulation_Previous = _fbTemporalAccumulation[(a_Renderer.frameIndex + 1) % 2];
    auto fbCompositingSize                = _fbCompositing->info.defaultSize;
    auto fbTemporalAccumulationSize       = fbTemporalAccumulation != nullptr ? fbTemporalAccumulation->info.defaultSize : glm::uvec3(0);
    auto& info                            = _renderPassTemporalAccumulationInfo;
    if (fbCompositingSize != fbTemporalAccumulationSize) {
        fbTemporalAccumulation = CreateFbTemporalAccumulation(a_Renderer.context, fbCompositingSize);
    }
    // FILL VIEWPORT STATES
    info.name                         = "TemporalAccumulation";
    info.viewportState                = _renderPassCompositingInfo.viewportState;
    info.frameBufferState             = { .framebuffer = fbTemporalAccumulation };
    info.frameBufferState.drawBuffers = { GL_COLOR_ATTACHMENT0 };
    // FILL GRAPHICS PIPELINES
    info.pipelines.clear();
    auto color_Previous                            = fbTemporalAccumulation_Previous != nullptr ? fbTemporalAccumulation_Previous->info.colorBuffers[0].texture : _fbOpaque->info.colorBuffers[OUTPUT_FRAG_FWD_OPAQUE_COLOR].texture;
    auto& gpInfo                                   = std::get<OGLGraphicsPipelineInfo>(info.pipelines.emplace_back());
    gpInfo.depthStencilState                       = { .enableDepthTest = false };
    gpInfo.shaderState                             = _shaderTemporalAccumulation;
    gpInfo.inputAssemblyState                      = { .primitiveTopology = GL_TRIANGLES };
    gpInfo.rasterizationState                      = { .cullMode = GL_NONE };
    gpInfo.vertexInputState                        = { .vertexCount = 3, .vertexArray = _presentVAO };
    gpInfo.bindings.textures[0]                    = { .texture = color_Previous, .sampler = _TAASampler };
    gpInfo.bindings.textures[1]                    = { .texture = _fbOpaque->info.colorBuffers[OUTPUT_FRAG_FWD_OPAQUE_COLOR].texture, .sampler = _TAASampler };
    gpInfo.bindings.textures[2]                    = { .texture = _fbOpaque->info.colorBuffers[OUTPUT_FRAG_FWD_OPAQUE_VELOCITY].texture, .sampler = _TAASampler };
    gpInfo.drawCommands.emplace_back().vertexCount = 3;
    // CREATE RENDER PASS
    renderPasses.emplace_back(new OGLRenderPass(info));
}

void PathFwd::_UpdateRenderPassPresent(Renderer::Impl& a_Renderer)
{
    auto& renderBuffer              = *a_Renderer.activeRenderBuffer;
    auto& fbTemporalAccumulation    = _fbTemporalAccumulation[a_Renderer.frameIndex % 2];
    auto fbTemporalAccumulationSize = fbTemporalAccumulation->info.defaultSize;
    auto fbPresentSize              = _fbPresent != nullptr ? _fbPresent->info.defaultSize : glm::uvec3(0);
    auto& info                      = _renderPassPresentInfo;
    if (fbTemporalAccumulationSize != fbPresentSize) {
        _fbPresent = CreateFbPresent(a_Renderer.context, fbTemporalAccumulationSize);
        // FILL VIEWPORT STATES
        info.name                         = "Present";
        info.viewportState                = _renderPassCompositingInfo.viewportState;
        info.frameBufferState             = { .framebuffer = _fbPresent };
        info.frameBufferState.drawBuffers = {};
    }
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
