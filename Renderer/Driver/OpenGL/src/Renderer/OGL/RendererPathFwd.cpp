#include <Renderer/OGL/Components/MeshData.hpp>
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
auto CreateFwdRenderPass(
    const glm::uvec2& a_Viewport,
    const std::shared_ptr<RAII::FrameBuffer>& a_FB,
    const glm::vec3& a_ClearColor = { 0, 0, 0 })
{
    RenderPassInfo info;
    info.name                         = "Forward";
    info.viewportState.viewport       = a_Viewport;
    info.frameBufferState.framebuffer = a_FB;
    info.frameBufferState.clear.colors.resize(OUTPUT_FRAG_COUNT);
    info.frameBufferState.clear.colors[OUTPUT_FRAG_MATERIAL] = { OUTPUT_FRAG_MATERIAL, { 0u, 0u } };
    info.frameBufferState.clear.colors[OUTPUT_FRAG_NORMAL]   = { OUTPUT_FRAG_NORMAL, { 0.f, 0.f, 0.f } };
    info.frameBufferState.clear.colors[OUTPUT_FRAG_VELOCITY] = { OUTPUT_FRAG_VELOCITY, { 0.f, 0.f } };
    info.frameBufferState.clear.colors[OUTPUT_FRAG_FINAL]    = { OUTPUT_FRAG_FINAL, { a_ClearColor.r, a_ClearColor.g, a_ClearColor.b } };
    info.frameBufferState.clear.depth                        = 1.f;
    info.frameBufferState.drawBuffers                        = {
        GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_MATERIAL,
        GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_NORMAL,
        GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_VELOCITY,
        GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_FINAL
    };
    return info;
}

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

auto CreatePresentShader(Renderer::Impl& a_Renderer)
{
    ShaderState shaderState;
    shaderState.program = a_Renderer.shaderCompiler.CompileProgram("Present");
    return shaderState;
}

auto CreatePresentRenderPass(
    Renderer::Impl& a_Renderer,
    const RenderBuffer::Handle& a_RenderBuffer,
    const std::shared_ptr<RAII::FrameBuffer>& a_FwdFB)
{
    auto& renderBuffer = *a_RenderBuffer;
    auto& context      = a_Renderer.context;
    RenderPassInfo info;
    info.name                   = "Present";
    info.viewportState.viewport = { renderBuffer->width, renderBuffer->height };
    info.frameBufferState       = {
              .framebuffer = RAII::MakePtr<RAII::FrameBuffer>(context, RAII::FrameBufferCreateInfo { .defaultSize = { info.viewportState.viewport, 1 } })
    };
    info.bindings.images = {
        ImageBindingInfo {
            .bindingIndex = 0,
            .texture      = a_FwdFB->info.colorBuffers[OUTPUT_FRAG_FINAL].texture,
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
            .shaderState        = CreatePresentShader(a_Renderer),
            .inputAssemblyState = { .primitiveTopology = GL_TRIANGLES },
            .rasterizationState = { .cullMode = GL_NONE },
            .vertexInputState   = { .vertexCount = 3, .vertexArray = CreatePresentVAO(context) } }
    };
    return info;
}

/**
 * Deferred Render Target :
 * RT0 : BRDF CDiff/BRDF Alpha (R), BRDF F0/AO (G) GL_RG32UI
 * RT2 : World Normal (RGB)                        GL_RGB16_SNORM
 * RT3 : Velocity (RG)                             GL_RG16F
 * RT4 : Color (Unlit/Emissive/Final Color)        GL_RGBA16F
 * Depth                                           GL_DEPTH_COMPONENT24
 */
auto CreateFwdFB(
    Context& a_Context,
    const glm::uvec2& a_Size)
{
    RAII::FrameBufferCreateInfo info;
    info.defaultSize = { a_Size, 1 };
    info.colorBuffers.resize(OUTPUT_FRAG_COUNT);
    info.colorBuffers[OUTPUT_FRAG_MATERIAL].attachment = GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_MATERIAL;
    info.colorBuffers[OUTPUT_FRAG_NORMAL].attachment   = GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_NORMAL;
    info.colorBuffers[OUTPUT_FRAG_VELOCITY].attachment = GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_VELOCITY;
    info.colorBuffers[OUTPUT_FRAG_FINAL].attachment    = GL_COLOR_ATTACHMENT0 + OUTPUT_FRAG_FINAL;
    info.colorBuffers[OUTPUT_FRAG_MATERIAL].texture    = RAII::MakePtr<RAII::Texture2D>(a_Context, a_Size.x, a_Size.y, 1, GL_RG32UI);
    info.colorBuffers[OUTPUT_FRAG_NORMAL].texture      = RAII::MakePtr<RAII::Texture2D>(a_Context, a_Size.x, a_Size.y, 1, GL_RGB16_SNORM);
    info.colorBuffers[OUTPUT_FRAG_VELOCITY].texture    = RAII::MakePtr<RAII::Texture2D>(a_Context, a_Size.x, a_Size.y, 1, GL_RG16F);
    info.colorBuffers[OUTPUT_FRAG_FINAL].texture       = RAII::MakePtr<RAII::Texture2D>(a_Context, a_Size.x, a_Size.y, 1, GL_RGBA16F);
    info.depthBuffer                                   = RAII::MakePtr<RAII::Texture2D>(a_Context, a_Size.x, a_Size.y, 1, GL_DEPTH_COMPONENT24);
    return RAII::MakePtr<RAII::FrameBuffer>(a_Context, info);
}

PathFwd::PathFwd(Renderer::Impl& a_Renderer, const RendererSettings& a_Settings)
    : _metRoughShader({ .program = a_Renderer.shaderCompiler.CompileProgram("ForwardLitMetRough") })
    , _specGlossShader({ .program = a_Renderer.shaderCompiler.CompileProgram("ForwardLitSpecGloss") })
    , _fb(CreateFwdFB(a_Renderer.context, { 2048, 2048 }))
    , _renderPass(_CreateRenderPass(CreateFwdRenderPass({ 2048, 2048 }, _fb)))

{
}

void PathFwd::Update(Renderer::Impl& a_Renderer)
{
    auto& activeScene  = a_Renderer.activeScene;
    auto& renderBuffer = *a_Renderer.activeRenderBuffer;
    auto FBSize        = _renderPass->info.frameBufferState.framebuffer->info.defaultSize;
    if (FBSize.x < renderBuffer->width || FBSize.y < renderBuffer->height) {
        _fb = CreateFwdFB(a_Renderer.context, FBSize * 2u);
    }
    RenderPassInfo passInfo                                         = _renderPass->info;
    passInfo.frameBufferState.framebuffer                           = _fb;
    passInfo.frameBufferState.clear.colors[OUTPUT_FRAG_FINAL].color = {
        activeScene->GetBackgroundColor().r, activeScene->GetBackgroundColor().g, activeScene->GetBackgroundColor().b
    };
    passInfo.viewportState.viewport      = { renderBuffer->width, renderBuffer->height };
    passInfo.viewportState.scissorExtent = passInfo.viewportState.viewport;
    passInfo.bindings.buffers            = {
        { GL_UNIFORM_BUFFER, UBO_CAMERA, a_Renderer.cameraUBO.buffer, 0, a_Renderer.cameraUBO.buffer->size },
        { GL_SHADER_STORAGE_BUFFER, SSBO_VTFS_LIGHTS, a_Renderer.lightCuller.GPUlightsBuffer, offsetof(GLSL::VTFSLightsBuffer, lights), a_Renderer.lightCuller.GPUlightsBuffer->size },
        { GL_SHADER_STORAGE_BUFFER, SSBO_VTFS_CLUSTERS, a_Renderer.lightCuller.GPUclusters, 0, a_Renderer.lightCuller.GPUclusters->size }
    };
    passInfo.bindings.textures = {
        { SAMPLERS_BRDF_LUT, GL_TEXTURE_2D, a_Renderer.BrdfLut, a_Renderer.BrdfLutSampler }
    };
    for (auto i = 0u; i < a_Renderer.lightCuller.iblSamplers.size(); i++) {
        passInfo.bindings.textures.push_back({ SAMPLERS_VTFS_IBL + i, GL_TEXTURE_CUBE_MAP, a_Renderer.lightCuller.iblSamplers.at(i), a_Renderer.IblSpecSampler });
    }
    _UpdateGraphicsPipelines(a_Renderer, passInfo.graphicsPipelines);
    _renderPass        = _CreateRenderPass(passInfo);
    _renderPassPresent = _CreateRenderPass(CreatePresentRenderPass(a_Renderer, a_Renderer.activeRenderBuffer, _fb));
}

void PathFwd::Execute()
{
    _renderPass->Execute();
    _renderPassPresent->Execute();
}

std::shared_ptr<RenderPass> PathFwd::_CreateRenderPass(const RenderPassInfo& a_Info)
{
    return std::shared_ptr<RenderPass>(
        new (renderPassMemoryPool.allocate()) RenderPass(a_Info),
        renderPassMemoryPool.deleter());
}

void PathFwd::_UpdateGraphicsPipelines(Renderer::Impl& a_Renderer, std::vector<GraphicsPipelineInfo>& a_GraphicsPipelines)
{
    auto& activeScene = a_Renderer.activeScene;
    a_GraphicsPipelines.clear();
    if (activeScene->GetSkybox().texture != nullptr) {
        auto skybox                              = a_Renderer.LoadTexture(activeScene->GetSkybox().texture.get());
        auto sampler                             = a_Renderer.LoadSampler(activeScene->GetSkybox().sampler.get());
        auto& graphicsPipelineInfo               = a_GraphicsPipelines.emplace_back();
        graphicsPipelineInfo.shaderState.program = a_Renderer.shaderCompiler.CompileProgram("Skybox");
        graphicsPipelineInfo.vertexInputState    = { .vertexCount = 3, .vertexArray = CreatePresentVAO(a_Renderer.context) };
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
        for (auto& [primitive, material] : rPrimitives) {
            auto& graphicsPipelineInfo = a_GraphicsPipelines.emplace_back();
            if (material->type == MATERIAL_TYPE_METALLIC_ROUGHNESS)
                graphicsPipelineInfo.shaderState = _metRoughShader;
            else if (material->type == MATERIAL_TYPE_SPECULAR_GLOSSINESS)
                graphicsPipelineInfo.shaderState = _specGlossShader;
            graphicsPipelineInfo.bindings.buffers = {
                { GL_UNIFORM_BUFFER, UBO_TRANSFORM, rTransform.buffer, 0, rTransform.buffer->size },
                { GL_UNIFORM_BUFFER, UBO_MATERIAL, material->buffer, 0, material->buffer->size },
            };
            for (uint32_t i = 0; i < material->textureSamplers.size(); ++i) {
                auto& textureSampler = material->textureSamplers.at(i);
                auto target          = textureSampler.texture != nullptr ? textureSampler.texture->target : GL_TEXTURE_2D;
                graphicsPipelineInfo.bindings.textures.push_back({ SAMPLERS_MATERIAL + i, target, textureSampler.texture, textureSampler.sampler });
            }
            graphicsPipelineInfo.inputAssemblyState.primitiveTopology = primitive->drawMode;
            graphicsPipelineInfo.vertexInputState.vertexArray         = primitive->vertexArray;
        }
    }
}
}