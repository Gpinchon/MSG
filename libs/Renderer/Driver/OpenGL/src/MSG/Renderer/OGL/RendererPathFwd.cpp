#include <MSG/Entity/Camera.hpp>
#include <MSG/Mesh.hpp>
#include <MSG/OGLBuffer.hpp>
#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLSampler.hpp>
#include <MSG/OGLTexture2D.hpp>
#include <MSG/OGLTextureCubemap.hpp>
#include <MSG/OGLVertexArray.hpp>
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
#include <MSG/Tools/BRDFIntegration.hpp>
#include <MSG/Tools/Halton.hpp>
#include <MSG/Transform.hpp>

#include <Material.glsl>

#include <glm/gtc/matrix_inverse.hpp>

#include <ranges>
#include <unordered_set>
#include <vector>

namespace MSG::Renderer {
template <unsigned Size>
glm::vec2 Halton23(const unsigned& a_Index)
{
    constexpr auto halton2 = Tools::Halton<2>::Sequence<Size>();
    constexpr auto halton3 = Tools::Halton<3>::Sequence<Size>();
    const auto rIndex      = a_Index % Size;
    return { halton2[rIndex], halton3[rIndex] };
}

static inline auto ApplyTemporalJitter(glm::mat4 a_ProjMat, const uint8_t& a_FrameIndex)
{
    // the jitter amount should go bellow the threshold of velocity buffer
    constexpr float f16lowest = 1 / 1024.f;
    auto halton               = (Halton23<256>(a_FrameIndex) * 0.5f + 0.5f) * f16lowest;
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
    info.colorBuffers[OUTPUT_FRAG_FWD_OPAQUE_COLOR].texture       = std::make_shared<OGLTexture2D>(a_Context, a_Size.x, a_Size.y, 1, GL_RGBA16F);
    info.colorBuffers[OUTPUT_FRAG_FWD_OPAQUE_VELOCITY].texture    = std::make_shared<OGLTexture2D>(a_Context, a_Size.x, a_Size.y, 1, GL_RG16F);
    info.depthBuffer                                              = std::make_shared<OGLTexture2D>(a_Context, a_Size.x, a_Size.y, 1, GL_DEPTH_COMPONENT24);
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
    const std::shared_ptr<OGLTexture2D>& a_OpaqueColor,
    const std::shared_ptr<OGLTexture2D>& a_OpaqueDepth)
{
    OGLFrameBufferCreateInfo info;
    info.defaultSize = { a_Size, 1 };
    info.colorBuffers.resize(OUTPUT_FRAG_FWD_BLENDED_COUNT);
    info.colorBuffers[OUTPUT_FRAG_FWD_BLENDED_ACCUM].attachment = GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_FWD_BLENDED_ACCUM;
    info.colorBuffers[OUTPUT_FRAG_FWD_BLENDED_REV].attachment   = GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_FWD_BLENDED_REV;
    info.colorBuffers[OUTPUT_FRAG_FWD_BLENDED_COLOR].attachment = GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_FWD_BLENDED_COLOR;
    info.colorBuffers[OUTPUT_FRAG_FWD_BLENDED_ACCUM].texture    = std::make_shared<OGLTexture2D>(a_Context, a_Size.x, a_Size.y, 1, GL_RGBA16F);
    info.colorBuffers[OUTPUT_FRAG_FWD_BLENDED_REV].texture      = std::make_shared<OGLTexture2D>(a_Context, a_Size.x, a_Size.y, 1, GL_R8);
    info.colorBuffers[OUTPUT_FRAG_FWD_BLENDED_COLOR].texture    = a_OpaqueColor;
    info.depthBuffer                                            = a_OpaqueDepth;
    return std::make_shared<OGLFrameBuffer>(a_Context, info);
}

auto CreateFbCompositing(
    OGLContext& a_Context,
    const glm::uvec2& a_Size,
    const std::shared_ptr<OGLTexture2D>& a_OpaqueColor)
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
    info.colorBuffers[0].texture    = std::make_shared<OGLTexture2D>(a_Context, a_Size.x, a_Size.y, 1, GL_RGBA16F);
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

OGLBindings PathFwd::_GetGlobalBindings() const
{
    OGLBindings bindings;
    bindings.uniformBuffers[UBO_CAMERA]         = { _cameraUBO.buffer, 0, _cameraUBO.buffer->size };
    bindings.uniformBuffers[UBO_FWD_IBL]        = { _iblUBO.buffer, 0, _iblUBO.buffer->size };
    bindings.storageBuffers[SSBO_VTFS_LIGHTS]   = { _lightCuller.vtfs.lightsBuffer, offsetof(GLSL::VTFSLightsBuffer, lights), _lightCuller.vtfs.lightsBuffer->size };
    bindings.storageBuffers[SSBO_VTFS_CLUSTERS] = { _lightCuller.vtfs.cluster, 0, _lightCuller.vtfs.cluster->size };
    bindings.textures[SAMPLERS_BRDF_LUT]        = { GL_TEXTURE_2D, _brdfLut, _brdfLutSampler };
    for (auto i = 0u; i < SAMPLERS_FWD_IBL_COUNT && i < _lightCuller.ibl.samplers.size(); i++) {
        bindings.textures[SAMPLERS_FWD_IBL + i] = { .target = GL_TEXTURE_CUBE_MAP, .texture = _lightCuller.ibl.samplers.at(i), .sampler = _iblSpecSampler };
    }
    return bindings;
}

void PathFwd::_UpdateCamera(Renderer::Impl& a_Renderer)
{
    auto& activeScene                = a_Renderer.activeScene;
    auto& currentCamera              = activeScene->GetCamera();
    GLSL::CameraUBO cameraUBOData    = _cameraUBO.GetData();
    cameraUBOData.previous           = cameraUBOData.current;
    cameraUBOData.current.position   = currentCamera.GetComponent<MSG::Transform>().GetWorldPosition();
    cameraUBOData.current.projection = currentCamera.GetComponent<Camera>().projection.GetMatrix();
    if (a_Renderer.enableTAA)
        cameraUBOData.current.projection = ApplyTemporalJitter(cameraUBOData.current.projection, uint8_t(a_Renderer.frameIndex));
    cameraUBOData.current.view = glm::inverse(currentCamera.GetComponent<MSG::Transform>().GetWorldTransformMatrix());
    _cameraUBO.SetData(cameraUBOData);
    if (_cameraUBO.needsUpdate)
        a_Renderer.uboToUpdate.emplace_back(_cameraUBO);
}

void PathFwd::_UpdateLights(Renderer::Impl& a_Renderer)
{
    _lightCuller(a_Renderer.activeScene, _cameraUBO.buffer);
    GLSL::FwdIBL ibl;
    ibl.count = std::min(_lightCuller.ibl.lights.size(), size_t(FWD_LIGHT_MAX_IBL));
    for (uint32_t i = 0; i < ibl.count; i++)
        ibl.lights[i] = _lightCuller.ibl.lights.at(i);
    _iblUBO.SetData(ibl);
    if (_iblUBO.needsUpdate)
        a_Renderer.uboToUpdate.emplace_back(_iblUBO);
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
        auto target                                   = textureSampler.texture != nullptr ? textureSampler.texture->target : GL_TEXTURE_2D;
        info.bindings.textures[SAMPLERS_MATERIAL + i] = { GLenum(target), textureSampler.texture, textureSampler.sampler };
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
    info.bindings.textures[SAMPLERS_SKYBOX] = { a_Skybox->target, a_Skybox, a_Sampler };
    return info;
}

auto GetStandardBRDF()
{
    Texture* brdfLutTexture = nullptr;
    if (brdfLutTexture != nullptr)
        return brdfLutTexture;
    glm::uvec3 LUTSize        = { 256, 256, 1 };
    PixelDescriptor pixelDesc = PixelSizedFormat::Uint8_NormalizedRGBA;
    auto brdfLutImage         = std::make_shared<Image>(pixelDesc, LUTSize.x, LUTSize.y, 1, std::make_shared<BufferView>(0, LUTSize.x * LUTSize.y * LUTSize.z * pixelDesc.GetPixelSize()));
    auto brdfIntegration      = Tools::BRDFIntegration::Generate(256, 256, Tools::BRDFIntegration::Type::Standard);
    for (uint32_t z = 0; z < LUTSize.z; ++z) {
        for (uint32_t y = 0; y < LUTSize.y; ++y) {
            for (uint32_t x = 0; x < LUTSize.x; ++x) {
                brdfLutImage->Store({ x, y, z }, { brdfIntegration[x][y], 0, 1 });
            }
        }
    }
    return brdfLutTexture = new Texture(TextureType::Texture2D, brdfLutImage);
}

PathFwd::PathFwd(Renderer::Impl& a_Renderer, const RendererSettings& a_Settings)
    : _lightCuller(a_Renderer, FWD_LIGHT_MAX_IBL, FWD_LIGHT_MAX_SHADOWS)
    , _cameraUBO(a_Renderer.context)
    , _iblUBO(a_Renderer.context)
    , _TAASampler(std::make_shared<OGLSampler>(a_Renderer.context, OGLSamplerParameters { .wrapS = GL_CLAMP_TO_EDGE, .wrapT = GL_CLAMP_TO_EDGE, .wrapR = GL_CLAMP_TO_EDGE }))
    , _iblSpecSampler(std::make_shared<OGLSampler>(a_Renderer.context, OGLSamplerParameters { .minFilter = GL_LINEAR_MIPMAP_LINEAR }))
    , _brdfLutSampler(std::make_shared<OGLSampler>(a_Renderer.context, OGLSamplerParameters { .wrapS = GL_CLAMP_TO_EDGE, .wrapT = GL_CLAMP_TO_EDGE, .wrapR = GL_CLAMP_TO_EDGE }))
    , _brdfLut(a_Renderer.LoadTexture(GetStandardBRDF()))
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
    _UpdateCamera(a_Renderer);
    _UpdateLights(a_Renderer);
    _UpdateRenderPassOpaque(a_Renderer);
    _UpdateRenderPassBlended(a_Renderer);
    _UpdateRenderPassCompositing(a_Renderer);
    _UpdateRenderPassTemporalAccumulation(a_Renderer);
    _UpdateRenderPassPresent(a_Renderer);
}

std::shared_ptr<OGLRenderPass> PathFwd::_CreateRenderPass(const OGLRenderPassInfo& a_Info)
{
    return std::shared_ptr<OGLRenderPass>(
        new (_renderPassMemoryPool.allocate()) OGLRenderPass(a_Info),
        _renderPassMemoryPool.deleter());
}

void PathFwd::_UpdateRenderPassShadows(Renderer::Impl& a_Renderer)
{
    // TODO: Implement this
}

void PathFwd::_UpdateRenderPassOpaque(Renderer::Impl& a_Renderer)
{
    auto& activeScene     = a_Renderer.activeScene;
    auto& renderBuffer    = *a_Renderer.activeRenderBuffer;
    auto renderBufferSize = glm::uvec3(renderBuffer->width, renderBuffer->height, 1);
    auto fbOpaqueSize     = _fbOpaque != nullptr ? _fbOpaque->info.defaultSize : glm::uvec3(0);
    auto& clearColor      = activeScene->GetBackgroundColor();
    auto globalBindings   = _GetGlobalBindings();
    if (fbOpaqueSize != renderBufferSize)
        _fbOpaque = CreateFbOpaque(a_Renderer.context, renderBufferSize);
    OGLRenderPassInfo info;
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
    if (activeScene->GetSkybox().texture != nullptr) { // first render Skybox if needed
        auto skybox  = a_Renderer.LoadTexture(activeScene->GetSkybox().texture.get());
        auto sampler = activeScene->GetSkybox().sampler != nullptr ? a_Renderer.LoadSampler(activeScene->GetSkybox().sampler.get()) : nullptr;
        info.graphicsPipelines.emplace_back(GetSkyboxGraphicsPipeline(a_Renderer, _presentVAO, globalBindings, skybox, sampler));
    }
    for (auto& entityRef : activeScene->GetVisibleEntities().meshes) {
        auto& rMesh      = entityRef.GetComponent<Component::Mesh>().at(entityRef.lod);
        auto& rTransform = entityRef.GetComponent<Component::Transform>();
        auto rMeshSkin   = entityRef.HasComponent<Component::MeshSkin>() ? &entityRef.GetComponent<Component::MeshSkin>() : nullptr;
        for (auto& [rPrimitive, rMaterial] : rMesh) {
            const bool isAlphaBlend = rMaterial->alphaMode == MATERIAL_ALPHA_BLEND;
            const bool isMetRough   = rMaterial->type == MATERIAL_TYPE_METALLIC_ROUGHNESS;
            const bool isSpecGloss  = rMaterial->type == MATERIAL_TYPE_SPECULAR_GLOSSINESS;
            const bool isUnlit      = rMaterial->unlit;
            // is there any chance we have opaque pixels here ?
            // it's ok to use specularGlossiness.diffuseFactor even with metrough because they share type/location
            if (isAlphaBlend && rMaterial->GetData().specularGlossiness.diffuseFactor.a < 1)
                continue;
            auto& graphicsPipelineInfo = info.graphicsPipelines.emplace_back(GetCommonGraphicsPipeline(globalBindings, *rPrimitive, *rMaterial, rTransform, rMeshSkin));
            if (isMetRough)
                graphicsPipelineInfo.shaderState = isUnlit ? _shaderMetRoughOpaqueUnlit : _shaderMetRoughOpaque;
            else if (isSpecGloss)
                graphicsPipelineInfo.shaderState = isUnlit ? _shaderSpecGlossOpaqueUnlit : _shaderSpecGlossOpaque;
        }
    }
    _renderPassOpaque = renderPasses.emplace_back(_CreateRenderPass(info));
}

void PathFwd::_UpdateRenderPassBlended(Renderer::Impl& a_Renderer)
{
    constexpr auto colorBlendStates = GetBlendedOGLColorBlendAttachmentState();
    auto& activeScene               = a_Renderer.activeScene;
    auto& fbOpaque                  = _fbOpaque;
    auto fbOpaqueSize               = fbOpaque->info.defaultSize;
    auto fbBlendSize                = _fbBlended != nullptr ? _fbBlended->info.defaultSize : glm::uvec3(0);
    auto globalBindings             = _GetGlobalBindings();
    if (fbOpaqueSize != fbBlendSize)
        _fbBlended = CreateFbBlended(
            a_Renderer.context, fbOpaqueSize,
            fbOpaque->info.colorBuffers[OUTPUT_FRAG_FWD_OPAQUE_COLOR].texture,
            fbOpaque->info.depthBuffer);
    OGLRenderPassInfo info;
    info.name                         = "FwdBlended";
    info.viewportState                = _renderPassOpaque.lock()->info.viewportState;
    info.frameBufferState.framebuffer = _fbBlended;
    info.frameBufferState.clear.colors.resize(OUTPUT_FRAG_FWD_BLENDED_COUNT);
    info.frameBufferState.clear.colors[OUTPUT_FRAG_FWD_BLENDED_ACCUM] = { OUTPUT_FRAG_FWD_BLENDED_ACCUM, { 0.f, 0.f, 0.f, 0.f } };
    info.frameBufferState.clear.colors[OUTPUT_FRAG_FWD_BLENDED_REV]   = { OUTPUT_FRAG_FWD_BLENDED_REV, { 1.f } };
    info.frameBufferState.drawBuffers                                 = {
        GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_FWD_BLENDED_ACCUM,
        GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_FWD_BLENDED_REV,
        GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_FWD_BLENDED_COLOR
    };
    for (auto& entityRef : activeScene->GetVisibleEntities().meshes | std::views::reverse) {
        auto& rMesh      = entityRef.GetComponent<Component::Mesh>().at(entityRef.lod);
        auto& rTransform = entityRef.GetComponent<Component::Transform>();
        auto rMeshSkin   = entityRef.HasComponent<Component::MeshSkin>() ? &entityRef.GetComponent<Component::MeshSkin>() : nullptr;
        for (auto& [rPrimitive, rMaterial] : rMesh) {
            const bool isAlphaBlend = rMaterial->alphaMode == MATERIAL_ALPHA_BLEND;
            const bool isMetRough   = rMaterial->type == MATERIAL_TYPE_METALLIC_ROUGHNESS;
            const bool isSpecGloss  = rMaterial->type == MATERIAL_TYPE_SPECULAR_GLOSSINESS;
            const bool isUnlit      = rMaterial->unlit;
            if (!isAlphaBlend)
                continue;
            auto& graphicsPipelineInfo                              = info.graphicsPipelines.emplace_back(GetCommonGraphicsPipeline(globalBindings, *rPrimitive, *rMaterial, rTransform, rMeshSkin));
            graphicsPipelineInfo.colorBlend.attachmentStates        = { colorBlendStates.begin(), colorBlendStates.end() };
            graphicsPipelineInfo.depthStencilState.enableDepthWrite = false;
            if (isMetRough)
                graphicsPipelineInfo.shaderState = isUnlit ? _shaderMetRoughBlendedUnlit : _shaderMetRoughBlended;
            else if (isSpecGloss)
                graphicsPipelineInfo.shaderState = isUnlit ? _shaderSpecGlossBlendedUnlit : _shaderSpecGlossBlended;
        }
    }
    _renderPassBlended = renderPasses.emplace_back(_CreateRenderPass(info));
}

void PathFwd::_UpdateRenderPassCompositing(Renderer::Impl& a_Renderer)
{
    auto fbBlendSize       = _fbBlended->info.defaultSize;
    auto fbCompositingSize = _fbCompositing != nullptr ? _fbCompositing->info.defaultSize : glm::uvec3(0);
    if (fbBlendSize != fbCompositingSize)
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
    OGLGraphicsPipelineInfo graphicsPipelineInfo;
    graphicsPipelineInfo.colorBlend         = { .attachmentStates = { blending } };
    graphicsPipelineInfo.depthStencilState  = { .enableDepthTest = false };
    graphicsPipelineInfo.shaderState        = _shaderCompositing;
    graphicsPipelineInfo.inputAssemblyState = { .primitiveTopology = GL_TRIANGLES };
    graphicsPipelineInfo.rasterizationState = { .cullMode = GL_NONE };
    graphicsPipelineInfo.vertexInputState   = { .vertexCount = 3, .vertexArray = _presentVAO };
    graphicsPipelineInfo.bindings.images[0] = { _fbBlended->info.colorBuffers[OUTPUT_FRAG_FWD_BLENDED_ACCUM].texture, GL_READ_ONLY, GL_RGBA16F };
    graphicsPipelineInfo.bindings.images[1] = { _fbBlended->info.colorBuffers[OUTPUT_FRAG_FWD_BLENDED_REV].texture, GL_READ_ONLY, GL_R8 };

    OGLRenderPassInfo info;
    info.name                         = "Compositing";
    info.viewportState                = _renderPassBlended.lock()->info.viewportState;
    info.frameBufferState             = { .framebuffer = _fbCompositing };
    info.frameBufferState.drawBuffers = { GL_COLOR_ATTACHMENT0 };
    info.graphicsPipelines            = { graphicsPipelineInfo };
    _renderPassCompositing            = renderPasses.emplace_back(_CreateRenderPass(info));
}

void PathFwd::_UpdateRenderPassTemporalAccumulation(Renderer::Impl& a_Renderer)
{
    auto& fbTemporalAccumulation          = _fbTemporalAccumulation[(a_Renderer.frameIndex + 0) % 2];
    auto& fbTemporalAccumulation_Previous = _fbTemporalAccumulation[(a_Renderer.frameIndex + 1) % 2];
    auto fbCompositingSize                = _fbCompositing->info.defaultSize;
    auto fbTemporalAccumulationSize       = fbTemporalAccumulation != nullptr ? fbTemporalAccumulation->info.defaultSize : glm::uvec3(0);
    if (fbCompositingSize != fbTemporalAccumulationSize)
        fbTemporalAccumulation = CreateFbTemporalAccumulation(a_Renderer.context, fbCompositingSize);
    auto color_Previous = fbTemporalAccumulation_Previous != nullptr ? fbTemporalAccumulation_Previous->info.colorBuffers[0].texture : _fbOpaque->info.colorBuffers[OUTPUT_FRAG_FWD_OPAQUE_COLOR].texture;
    OGLRenderPassInfo info;
    info.name                                 = "TemporalAccumulation";
    info.viewportState                        = _renderPassCompositing.lock()->info.viewportState;
    info.frameBufferState                     = { .framebuffer = fbTemporalAccumulation };
    info.frameBufferState.drawBuffers         = { GL_COLOR_ATTACHMENT0 };
    auto& graphicsPipelineInfo                = info.graphicsPipelines.emplace_back();
    graphicsPipelineInfo.depthStencilState    = { .enableDepthTest = false };
    graphicsPipelineInfo.shaderState          = _shaderTemporalAccumulation;
    graphicsPipelineInfo.inputAssemblyState   = { .primitiveTopology = GL_TRIANGLES };
    graphicsPipelineInfo.rasterizationState   = { .cullMode = GL_NONE };
    graphicsPipelineInfo.vertexInputState     = { .vertexCount = 3, .vertexArray = _presentVAO };
    graphicsPipelineInfo.bindings.textures[0] = { .target = GL_TEXTURE_2D, .texture = color_Previous, .sampler = _TAASampler };
    graphicsPipelineInfo.bindings.textures[1] = { .target = GL_TEXTURE_2D, .texture = _fbOpaque->info.colorBuffers[OUTPUT_FRAG_FWD_OPAQUE_COLOR].texture, .sampler = _TAASampler };
    graphicsPipelineInfo.bindings.textures[2] = { .target = GL_TEXTURE_2D, .texture = _fbOpaque->info.colorBuffers[OUTPUT_FRAG_FWD_OPAQUE_VELOCITY].texture, .sampler = _TAASampler };
    _renderPassTemporalAccumulation           = renderPasses.emplace_back(_CreateRenderPass(info));
}

void PathFwd::_UpdateRenderPassPresent(Renderer::Impl& a_Renderer)
{
    auto& renderBuffer              = *a_Renderer.activeRenderBuffer;
    auto& fbTemporalAccumulation    = _fbTemporalAccumulation[a_Renderer.frameIndex % 2];
    auto fbTemporalAccumulationSize = fbTemporalAccumulation->info.defaultSize;
    auto fbPresentSize              = _fbPresent != nullptr ? _fbPresent->info.defaultSize : glm::uvec3(0);
    if (fbTemporalAccumulationSize != fbPresentSize)
        _fbPresent = CreateFbPresent(a_Renderer.context, fbTemporalAccumulationSize);

    OGLGraphicsPipelineInfo graphicsPipelineInfo;
    graphicsPipelineInfo.depthStencilState  = { .enableDepthTest = false };
    graphicsPipelineInfo.shaderState        = _shaderPresent;
    graphicsPipelineInfo.inputAssemblyState = { .primitiveTopology = GL_TRIANGLES };
    graphicsPipelineInfo.rasterizationState = { .cullMode = GL_NONE };
    graphicsPipelineInfo.vertexInputState   = { .vertexCount = 3, .vertexArray = _presentVAO };
    graphicsPipelineInfo.bindings.images[0] = { fbTemporalAccumulation->info.colorBuffers[0].texture, GL_READ_ONLY, GL_RGBA16F };
    graphicsPipelineInfo.bindings.images[1] = { renderBuffer, GL_WRITE_ONLY, GL_RGBA8 };

    OGLRenderPassInfo info;
    info.name                         = "Present";
    info.viewportState                = _renderPassCompositing.lock()->info.viewportState;
    info.frameBufferState             = { .framebuffer = _fbPresent };
    info.frameBufferState.drawBuffers = {};
    info.graphicsPipelines            = { graphicsPipelineInfo };
    _renderPassPresent                = renderPasses.emplace_back(_CreateRenderPass(info));
}
}
