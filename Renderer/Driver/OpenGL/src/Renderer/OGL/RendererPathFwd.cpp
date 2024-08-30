#include <Renderer/OGL/Components/MeshData.hpp>
#include <Renderer/OGL/Components/MeshSkin.hpp>
#include <Renderer/OGL/Components/Transform.hpp>
#include <Renderer/OGL/Material.hpp>
#include <Renderer/OGL/Primitive.hpp>
#include <Renderer/OGL/RAII/Buffer.hpp>
#include <Renderer/OGL/RAII/FrameBuffer.hpp>
#include <Renderer/OGL/RAII/VertexArray.hpp>
#include <Renderer/OGL/RenderBuffer.hpp>
#include <Renderer/OGL/Renderer.hpp>
#include <Renderer/OGL/RendererPathFwd.hpp>

#include <Material.glsl>

#include <SG/Component/Mesh.hpp>
#include <SG/Component/Transform.hpp>
#include <SG/Core/Image/Cubemap.hpp>
#include <SG/Scene/Scene.hpp>

#include <glm/gtc/matrix_inverse.hpp>

#include <unordered_set>
#include <vector>

namespace TabGraph::Renderer {
auto CreatePresentVAO(Context& a_Context)
{
    VertexAttributeDescription attribDesc {};
    attribDesc.binding           = 0;
    attribDesc.format.normalized = false;
    attribDesc.format.size       = 1;
    attribDesc.format.type       = GL_BYTE;
    VertexBindingDescription bindingDesc {};
    bindingDesc.buffer = RAII::MakePtr<RAII::Buffer>(a_Context, 3, nullptr, 0);
    bindingDesc.index  = 0;
    bindingDesc.offset = 0;
    bindingDesc.stride = 1;
    std::vector<VertexAttributeDescription> attribs { attribDesc };
    std::vector<VertexBindingDescription> bindings { bindingDesc };
    return RAII::MakePtr<RAII::VertexArray>(a_Context,
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
    Context& a_Context,
    const glm::uvec2& a_Size)
{
    RAII::FrameBufferCreateInfo info;
    info.defaultSize = { a_Size, 1 };
    info.colorBuffers.resize(OUTPUT_FRAG_FWD_OPAQUE_COUNT);
    info.colorBuffers[OUTPUT_FRAG_FWD_OPAQUE_COLOR].attachment    = GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_FWD_OPAQUE_COLOR;
    info.colorBuffers[OUTPUT_FRAG_FWD_OPAQUE_VELOCITY].attachment = GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_FWD_OPAQUE_VELOCITY;
    info.colorBuffers[OUTPUT_FRAG_FWD_OPAQUE_COLOR].texture       = RAII::MakePtr<RAII::Texture2D>(a_Context, a_Size.x, a_Size.y, 1, GL_RGBA16F);
    info.colorBuffers[OUTPUT_FRAG_FWD_OPAQUE_VELOCITY].texture    = RAII::MakePtr<RAII::Texture2D>(a_Context, a_Size.x, a_Size.y, 1, GL_RG16F);
    info.depthBuffer                                              = RAII::MakePtr<RAII::Texture2D>(a_Context, a_Size.x, a_Size.y, 1, GL_DEPTH_COMPONENT24);
    return RAII::MakePtr<RAII::FrameBuffer>(a_Context, info);
}

/**
 * Transparent Render Target :
 * RT0 : Accum     GL_RGBA16F
 * RT2 : Revealage GL_R8
 * RT3 : Color     GL_RGB8
 */
auto CreateFbBlended(
    Context& a_Context,
    const glm::uvec2& a_Size,
    const std::shared_ptr<RAII::Texture2D>& a_OpaqueColor,
    const std::shared_ptr<RAII::Texture2D>& a_OpaqueDepth)
{
    RAII::FrameBufferCreateInfo info;
    info.defaultSize = { a_Size, 1 };
    info.colorBuffers.resize(OUTPUT_FRAG_FWD_BLENDED_COUNT);
    info.colorBuffers[OUTPUT_FRAG_FWD_BLENDED_ACCUM].attachment = GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_FWD_BLENDED_ACCUM;
    info.colorBuffers[OUTPUT_FRAG_FWD_BLENDED_REV].attachment   = GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_FWD_BLENDED_REV;
    info.colorBuffers[OUTPUT_FRAG_FWD_BLENDED_COLOR].attachment = GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_FWD_BLENDED_COLOR;
    info.colorBuffers[OUTPUT_FRAG_FWD_BLENDED_ACCUM].texture    = RAII::MakePtr<RAII::Texture2D>(a_Context, a_Size.x, a_Size.y, 1, GL_RGBA16F);
    info.colorBuffers[OUTPUT_FRAG_FWD_BLENDED_REV].texture      = RAII::MakePtr<RAII::Texture2D>(a_Context, a_Size.x, a_Size.y, 1, GL_R8);
    info.colorBuffers[OUTPUT_FRAG_FWD_BLENDED_COLOR].texture    = a_OpaqueColor;
    info.depthBuffer                                            = a_OpaqueDepth;
    return RAII::MakePtr<RAII::FrameBuffer>(a_Context, info);
}

auto CreateFbCompositing(
    Context& a_Context,
    const glm::uvec2& a_Size,
    const std::shared_ptr<RAII::Texture2D>& a_OpaqueColor)
{
    RAII::FrameBufferCreateInfo info;
    info.defaultSize = { a_Size, 1 };
    info.colorBuffers.resize(1);
    info.colorBuffers[OUTPUT_FRAG_FWD_COMP_COLOR].attachment = GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_FWD_COMP_COLOR;
    info.colorBuffers[OUTPUT_FRAG_FWD_COMP_COLOR].texture    = a_OpaqueColor;
    return RAII::MakePtr<RAII::FrameBuffer>(a_Context, info);
}

auto CreateFbTemporalAccumulation(
    Context& a_Context,
    const glm::uvec2& a_Size)
{
    RAII::FrameBufferCreateInfo info;
    info.defaultSize = { a_Size, 1 };
    info.colorBuffers.resize(1);
    info.colorBuffers[0].attachment = GL_COLOR_ATTACHMENT0 + 0;
    info.colorBuffers[0].texture    = RAII::MakePtr<RAII::Texture2D>(a_Context, a_Size.x, a_Size.y, 1, GL_RGBA16F);
    return RAII::MakePtr<RAII::FrameBuffer>(a_Context, info);
}

auto CreateFbPresent(
    Context& a_Context,
    const glm::uvec2& a_Size)
{
    RAII::FrameBufferCreateInfo info;
    info.defaultSize = { a_Size, 1 };
    return RAII::MakePtr<RAII::FrameBuffer>(a_Context, info);
}

PathFwd::PathFwd(Renderer::Impl& a_Renderer, const RendererSettings& a_Settings)
    : _shaderMetRoughOpaque({ .program = a_Renderer.shaderCompiler.CompileProgram("FwdMetRough_Opaque") })
    , _shaderSpecGlossOpaque({ .program = a_Renderer.shaderCompiler.CompileProgram("FwdSpecGloss_Opaque") })
    , _shaderMetRoughBlended({ .program = a_Renderer.shaderCompiler.CompileProgram("FwdMetRough_Blended") })
    , _shaderSpecGlossBlended({ .program = a_Renderer.shaderCompiler.CompileProgram("FwdSpecGloss_Blended") })
    , _shaderCompositing({ .program = a_Renderer.shaderCompiler.CompileProgram("Compositing") })
    , _shaderTemporalAccumulation({ .program = a_Renderer.shaderCompiler.CompileProgram("TemporalAccumulation") })
    , _shaderPresent({ .program = a_Renderer.shaderCompiler.CompileProgram("Present") })
    , _presentVAO(CreatePresentVAO(a_Renderer.context))
{
}

void PathFwd::Update(Renderer::Impl& a_Renderer)
{
    _UpdateRenderPassOpaque(a_Renderer);
    _UpdateRenderPassBlended(a_Renderer);
    _UpdateRenderPassCompositing(a_Renderer);
    _UpdateRenderPassTemporalAccumulation(a_Renderer);
    _UpdateRenderPassPresent(a_Renderer);
    renderPasses = {
        _renderPassOpaque,
        _renderPassBlended,
        _renderPassCompositing,
        _renderPassTemporalAccumulation,
        _renderPassPresent
    };
}

std::shared_ptr<RenderPass> PathFwd::_CreateRenderPass(const RenderPassInfo& a_Info)
{
    return std::shared_ptr<RenderPass>(
        new (_renderPassMemoryPool.allocate()) RenderPass(a_Info),
        _renderPassMemoryPool.deleter());
}

void PathFwd::_UpdateRenderPassOpaque(Renderer::Impl& a_Renderer)
{
    auto& activeScene     = a_Renderer.activeScene;
    auto& renderBuffer    = *a_Renderer.activeRenderBuffer;
    auto renderBufferSize = glm::uvec3(renderBuffer->width, renderBuffer->height, 1);
    auto fbOpaqueSize     = _fbOpaque != nullptr ? _fbOpaque->info.defaultSize : glm::uvec3(0);
    if (fbOpaqueSize != renderBufferSize)
        _fbOpaque = CreateFbOpaque(a_Renderer.context, renderBufferSize);
    auto& clearColor = activeScene->GetBackgroundColor();
    RenderPassInfo info;
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
    info.bindings.buffers = {
        { GL_UNIFORM_BUFFER, UBO_CAMERA, a_Renderer.cameraUBO.buffer, 0, a_Renderer.cameraUBO.buffer->size },
        { GL_SHADER_STORAGE_BUFFER, SSBO_VTFS_LIGHTS, a_Renderer.lightCuller.GPUlightsBuffer, offsetof(GLSL::VTFSLightsBuffer, lights), a_Renderer.lightCuller.GPUlightsBuffer->size },
        { GL_SHADER_STORAGE_BUFFER, SSBO_VTFS_CLUSTERS, a_Renderer.lightCuller.GPUclusters, 0, a_Renderer.lightCuller.GPUclusters->size }
    };
    info.bindings.textures = {
        { SAMPLERS_BRDF_LUT, GL_TEXTURE_2D, a_Renderer.BrdfLut, a_Renderer.BrdfLutSampler }
    };
    for (auto i = 0u; i < a_Renderer.lightCuller.iblSamplers.size(); i++) {
        info.bindings.textures.push_back({ SAMPLERS_VTFS_IBL + i, GL_TEXTURE_CUBE_MAP, a_Renderer.lightCuller.iblSamplers.at(i), a_Renderer.IblSpecSampler });
    }
    info.graphicsPipelines.clear();
    if (activeScene->GetSkybox().texture != nullptr) {
        auto skybox                              = a_Renderer.LoadTexture(activeScene->GetSkybox().texture.get());
        auto sampler                             = a_Renderer.LoadSampler(activeScene->GetSkybox().sampler.get());
        auto& graphicsPipelineInfo               = info.graphicsPipelines.emplace_back();
        graphicsPipelineInfo.shaderState.program = a_Renderer.shaderCompiler.CompileProgram("Skybox");
        graphicsPipelineInfo.vertexInputState    = { .vertexCount = 3, .vertexArray = _presentVAO };
        graphicsPipelineInfo.inputAssemblyState  = { .primitiveTopology = GL_TRIANGLES };
        graphicsPipelineInfo.depthStencilState   = { .enableDepthTest = false };
        graphicsPipelineInfo.rasterizationState  = { .cullMode = GL_NONE };
        graphicsPipelineInfo.bindings.buffers    = {
            { GL_UNIFORM_BUFFER, UBO_CAMERA, a_Renderer.cameraUBO.buffer, 0, a_Renderer.cameraUBO.buffer->size }
        };
        graphicsPipelineInfo.bindings.textures = {
            { SAMPLERS_SKYBOX, skybox->target, skybox, sampler }
        };
    }
    auto view = activeScene->GetRegistry()->GetView<Component::PrimitiveList, Component::Transform>();
    for (const auto& [entityID, rPrimitives, rTransform] : view) {
        auto entityRef = activeScene->GetRegistry()->GetEntityRef(entityID);
        auto skinned   = entityRef.HasComponent<Component::MeshSkin>();
        for (auto& [primitive, material] : rPrimitives) {
            const bool isAlphaBlend  = material->alphaMode == MATERIAL_ALPHA_BLEND;
            const bool isMetRough    = material->type == MATERIAL_TYPE_METALLIC_ROUGHNESS;
            const bool isSpecGloss   = material->type == MATERIAL_TYPE_SPECULAR_GLOSSINESS;
            const bool isDoubleSided = material->doubleSided;
            // is there any chance we have opaque pixels here ?
            // it's ok to use specularGlossiness.diffuseFactor even with metrough because they share type/location
            if (isAlphaBlend && material->GetData().specularGlossiness.diffuseFactor.a < 1)
                continue;
            auto& graphicsPipelineInfo            = info.graphicsPipelines.emplace_back();
            graphicsPipelineInfo.bindings.buffers = {
                { GL_UNIFORM_BUFFER, UBO_TRANSFORM, rTransform.buffer, 0, rTransform.buffer->size },
                { GL_UNIFORM_BUFFER, UBO_MATERIAL, material->buffer, 0, material->buffer->size },
            };
            if (skinned) {
                auto& meshSkin = entityRef.GetComponent<Component::MeshSkin>();
                graphicsPipelineInfo.bindings.buffers.emplace_back(
                    GL_SHADER_STORAGE_BUFFER, SSBO_MESH_SKIN, meshSkin.buffer, 0, meshSkin.buffer->size);
                graphicsPipelineInfo.bindings.buffers.emplace_back(
                    GL_SHADER_STORAGE_BUFFER, SSBO_MESH_SKIN_PREV, meshSkin.buffer_Previous, 0, meshSkin.buffer_Previous->size);
            }
            if (isMetRough)
                graphicsPipelineInfo.shaderState = _shaderMetRoughOpaque;
            else if (isSpecGloss)
                graphicsPipelineInfo.shaderState = _shaderSpecGlossOpaque;
            if (isDoubleSided)
                graphicsPipelineInfo.rasterizationState.cullMode = GL_NONE;
            for (uint32_t i = 0; i < material->textureSamplers.size(); ++i) {
                auto& textureSampler = material->textureSamplers.at(i);
                auto target          = textureSampler.texture != nullptr ? textureSampler.texture->target : GL_TEXTURE_2D;
                graphicsPipelineInfo.bindings.textures.push_back({ SAMPLERS_MATERIAL + i, GLenum(target), textureSampler.texture, textureSampler.sampler });
            }
            graphicsPipelineInfo.inputAssemblyState.primitiveTopology = primitive->drawMode;
            graphicsPipelineInfo.vertexInputState.vertexArray         = primitive->vertexArray;
        }
    }
    _renderPassOpaque = _CreateRenderPass(info);
}

void PathFwd::_UpdateRenderPassBlended(Renderer::Impl& a_Renderer)
{
    auto& activeScene = a_Renderer.activeScene;
    auto& fbOpaque    = _fbOpaque;
    auto fbOpaqueSize = fbOpaque->info.defaultSize;
    auto fbBlendSize  = _fbBlended != nullptr ? _fbBlended->info.defaultSize : glm::uvec3(0);
    if (fbOpaqueSize != fbBlendSize)
        _fbBlended = CreateFbBlended(
            a_Renderer.context, fbOpaqueSize,
            fbOpaque->info.colorBuffers[OUTPUT_FRAG_FWD_OPAQUE_COLOR].texture,
            fbOpaque->info.depthBuffer);
    RenderPassInfo info;
    info.name                         = "FwdBlended";
    info.viewportState                = _renderPassOpaque->info.viewportState;
    info.frameBufferState.framebuffer = _fbBlended;
    info.frameBufferState.clear.colors.resize(OUTPUT_FRAG_FWD_BLENDED_COUNT);
    info.frameBufferState.clear.colors[OUTPUT_FRAG_FWD_BLENDED_ACCUM] = { OUTPUT_FRAG_FWD_BLENDED_ACCUM, { 0.f, 0.f, 0.f, 0.f } };
    info.frameBufferState.clear.colors[OUTPUT_FRAG_FWD_BLENDED_REV]   = { OUTPUT_FRAG_FWD_BLENDED_REV, { 1.f } };
    info.frameBufferState.drawBuffers                                 = {
        GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_FWD_BLENDED_ACCUM,
        GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_FWD_BLENDED_REV,
        GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_FWD_BLENDED_COLOR
    };
    info.bindings = _renderPassOpaque->info.bindings;
    auto view     = activeScene->GetRegistry()->GetView<Component::PrimitiveList, Component::Transform>();
    for (const auto& [entityID, rPrimitives, rTransform] : view) {
        auto entityRef = activeScene->GetRegistry()->GetEntityRef(entityID);
        auto skinned   = entityRef.HasComponent<Component::MeshSkin>();
        for (auto& [primitive, material] : rPrimitives) {
            if (material->alphaMode != MATERIAL_ALPHA_BLEND)
                continue;
            auto& graphicsPipelineInfo            = info.graphicsPipelines.emplace_back();
            graphicsPipelineInfo.bindings.buffers = {
                { GL_UNIFORM_BUFFER, UBO_TRANSFORM, rTransform.buffer, 0, rTransform.buffer->size },
                { GL_UNIFORM_BUFFER, UBO_MATERIAL, material->buffer, 0, material->buffer->size },
            };
            if (skinned) {
                auto& meshSkin = entityRef.GetComponent<Component::MeshSkin>();
                graphicsPipelineInfo.bindings.buffers.emplace_back(
                    GL_SHADER_STORAGE_BUFFER, SSBO_MESH_SKIN, meshSkin.buffer, 0, meshSkin.buffer->size);
            }
            if (material->type == MATERIAL_TYPE_METALLIC_ROUGHNESS)
                graphicsPipelineInfo.shaderState = _shaderMetRoughBlended;
            else if (material->type == MATERIAL_TYPE_SPECULAR_GLOSSINESS)
                graphicsPipelineInfo.shaderState = _shaderSpecGlossBlended;
            ColorBlendAttachmentState blendAccum;
            blendAccum.index               = OUTPUT_FRAG_FWD_BLENDED_ACCUM;
            blendAccum.enableBlend         = true;
            blendAccum.srcColorBlendFactor = GL_ONE;
            blendAccum.srcAlphaBlendFactor = GL_ONE;
            blendAccum.dstColorBlendFactor = GL_ONE;
            blendAccum.dstAlphaBlendFactor = GL_ONE;
            ColorBlendAttachmentState blendRev;
            blendRev.index               = OUTPUT_FRAG_FWD_BLENDED_REV;
            blendRev.enableBlend         = true;
            blendRev.srcColorBlendFactor = GL_ZERO;
            blendRev.srcAlphaBlendFactor = GL_ZERO;
            blendRev.dstColorBlendFactor = GL_ONE_MINUS_SRC_COLOR;
            blendRev.dstAlphaBlendFactor = GL_ONE_MINUS_SRC_COLOR;
            ColorBlendAttachmentState blendColor;
            blendColor.index                                 = OUTPUT_FRAG_FWD_BLENDED_COLOR;
            blendColor.enableBlend                           = true;
            blendColor.srcColorBlendFactor                   = GL_ZERO;
            blendColor.srcAlphaBlendFactor                   = GL_ZERO;
            blendColor.dstColorBlendFactor                   = GL_ONE_MINUS_SRC_COLOR;
            blendColor.dstAlphaBlendFactor                   = GL_ONE_MINUS_SRC_COLOR;
            graphicsPipelineInfo.colorBlend.attachmentStates = {
                blendAccum, blendRev, blendColor
            };
            if (material->doubleSided)
                graphicsPipelineInfo.rasterizationState.cullMode = GL_NONE;
            for (uint32_t i = 0; i < material->textureSamplers.size(); ++i) {
                auto& textureSampler = material->textureSamplers.at(i);
                auto target          = textureSampler.texture != nullptr ? textureSampler.texture->target : GL_TEXTURE_2D;
                graphicsPipelineInfo.bindings.textures.push_back({ SAMPLERS_MATERIAL + i, GLenum(target), textureSampler.texture, textureSampler.sampler });
            }
            graphicsPipelineInfo.depthStencilState.enableDepthWrite   = false;
            graphicsPipelineInfo.inputAssemblyState.primitiveTopology = primitive->drawMode;
            graphicsPipelineInfo.vertexInputState.vertexArray         = primitive->vertexArray;
        }
    }
    _renderPassBlended = _CreateRenderPass(info);
}

void PathFwd::_UpdateRenderPassCompositing(Renderer::Impl& a_Renderer)
{
    auto fbBlendSize       = _fbBlended->info.defaultSize;
    auto fbCompositingSize = _fbCompositing != nullptr ? _fbCompositing->info.defaultSize : glm::uvec3(0);
    if (fbBlendSize != fbCompositingSize)
        _fbCompositing = CreateFbCompositing(
            a_Renderer.context, fbBlendSize,
            _fbBlended->info.colorBuffers[OUTPUT_FRAG_FWD_BLENDED_COLOR].texture);
    RenderPassInfo info;
    info.name                         = "Compositing";
    info.viewportState                = _renderPassBlended->info.viewportState;
    info.frameBufferState             = { .framebuffer = _fbCompositing };
    info.frameBufferState.drawBuffers = { GL_COLOR_ATTACHMENT0 };
    info.bindings.images              = {
        ImageBindingInfo {
                         .bindingIndex = 0,
                         .texture      = _fbBlended->info.colorBuffers[OUTPUT_FRAG_FWD_BLENDED_ACCUM].texture,
                         .access       = GL_READ_ONLY,
                         .format       = GL_RGBA16F },
        ImageBindingInfo {
                         .bindingIndex = 1,
                         .texture      = _fbBlended->info.colorBuffers[OUTPUT_FRAG_FWD_BLENDED_REV].texture,
                         .access       = GL_READ_ONLY,
                         .format       = GL_R8 }
    };
    ColorBlendAttachmentState blending;
    blending.index               = OUTPUT_FRAG_FWD_COMP_COLOR;
    blending.enableBlend         = true;
    blending.srcColorBlendFactor = GL_ONE_MINUS_SRC_ALPHA;
    blending.srcAlphaBlendFactor = GL_ONE_MINUS_SRC_ALPHA;
    blending.dstColorBlendFactor = GL_ONE;
    blending.dstAlphaBlendFactor = GL_ONE;
    info.graphicsPipelines       = {
        GraphicsPipelineInfo {
                  .colorBlend         = { .attachmentStates = { blending } },
                  .depthStencilState  = { .enableDepthTest = false },
                  .shaderState        = _shaderCompositing,
                  .inputAssemblyState = { .primitiveTopology = GL_TRIANGLES },
                  .rasterizationState = { .cullMode = GL_NONE },
                  .vertexInputState   = { .vertexCount = 3, .vertexArray = _presentVAO } }
    };
    _renderPassCompositing = _CreateRenderPass(info);
}

void PathFwd::_UpdateRenderPassTemporalAccumulation(Renderer::Impl& a_Renderer)
{
    auto& fbTemporalAccumulation          = _fbTemporalAccumulation[(a_Renderer.frameIndex + 0) % 2];
    auto& fbTemporalAccumulation_Previous = _fbTemporalAccumulation[(a_Renderer.frameIndex + 1) % 2];
    auto fbCompositingSize                = _fbCompositing->info.defaultSize;
    auto fbTemporalAccumulationSize       = fbTemporalAccumulation != nullptr ? fbTemporalAccumulation->info.defaultSize : glm::uvec3(0);
    if (fbCompositingSize != fbTemporalAccumulationSize)
        fbTemporalAccumulation = CreateFbTemporalAccumulation(
            a_Renderer.context, fbCompositingSize);
    auto color_Previous = fbTemporalAccumulation_Previous != nullptr ? fbTemporalAccumulation_Previous->info.colorBuffers[0].texture : nullptr;
    RenderPassInfo info;
    info.name                         = "TemporalAccumulation";
    info.viewportState                = _renderPassCompositing->info.viewportState;
    info.frameBufferState             = { .framebuffer = fbTemporalAccumulation };
    info.frameBufferState.drawBuffers = { GL_COLOR_ATTACHMENT0 };
    info.bindings.textures            = {
        TextureBindingInfo {
                       .bindingIndex = 0,
                       .target       = GL_TEXTURE_2D,
                       .texture      = color_Previous }
    };
    info.bindings.images = {
        ImageBindingInfo {
            .bindingIndex = 0,
            .texture      = _fbOpaque->info.colorBuffers[OUTPUT_FRAG_FWD_OPAQUE_COLOR].texture,
            .access       = GL_READ_ONLY,
            .format       = GL_RGBA16F },
        ImageBindingInfo {
            .bindingIndex = 1,
            .texture      = _fbOpaque->info.colorBuffers[OUTPUT_FRAG_FWD_OPAQUE_VELOCITY].texture,
            .access       = GL_READ_ONLY,
            .format       = GL_RG16F }
    };
    info.graphicsPipelines = {
        GraphicsPipelineInfo {
            .depthStencilState  = { .enableDepthTest = false },
            .shaderState        = _shaderTemporalAccumulation,
            .inputAssemblyState = { .primitiveTopology = GL_TRIANGLES },
            .rasterizationState = { .cullMode = GL_NONE },
            .vertexInputState   = { .vertexCount = 3, .vertexArray = _presentVAO } }
    };
    _renderPassTemporalAccumulation = _CreateRenderPass(info);
}

void PathFwd::_UpdateRenderPassPresent(Renderer::Impl& a_Renderer)
{
    auto& renderBuffer              = *a_Renderer.activeRenderBuffer;
    auto& fbTemporalAccumulation    = _fbTemporalAccumulation[a_Renderer.frameIndex % 2];
    auto fbTemporalAccumulationSize = fbTemporalAccumulation->info.defaultSize;
    auto fbPresentSize              = _fbPresent != nullptr ? _fbPresent->info.defaultSize : glm::uvec3(0);
    if (fbTemporalAccumulationSize != fbPresentSize)
        _fbPresent = CreateFbPresent(a_Renderer.context, fbTemporalAccumulationSize);
    RenderPassInfo info;
    info.name                         = "Present";
    info.viewportState                = _renderPassCompositing->info.viewportState;
    info.frameBufferState             = { .framebuffer = _fbPresent };
    info.frameBufferState.drawBuffers = {};
    info.bindings.images              = {
        ImageBindingInfo {
                         .bindingIndex = 0,
                         .texture      = fbTemporalAccumulation->info.colorBuffers[0].texture,
                         .access       = GL_READ_ONLY,
                         .format       = GL_RGBA16F },
        ImageBindingInfo {
                         .bindingIndex = 1,
                         .texture      = renderBuffer,
                         .access       = GL_WRITE_ONLY,
                         .format       = GL_RGBA8 }
    };
    info.graphicsPipelines = {
        GraphicsPipelineInfo {
            .depthStencilState  = { .enableDepthTest = false },
            .shaderState        = _shaderPresent,
            .inputAssemblyState = { .primitiveTopology = GL_TRIANGLES },
            .rasterizationState = { .cullMode = GL_NONE },
            .vertexInputState   = { .vertexCount = 3, .vertexArray = _presentVAO } }
    };
    _renderPassPresent = _CreateRenderPass(info);
}
}
