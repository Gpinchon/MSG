#include <MSG/Renderer/OGL/Components/LightShadowData.hpp>
#include <MSG/Renderer/OGL/Components/MaterialSet.hpp>
#include <MSG/Renderer/OGL/Components/Mesh.hpp>
#include <MSG/Renderer/OGL/Components/MeshSkin.hpp>
#include <MSG/Renderer/OGL/Material.hpp>
#include <MSG/Renderer/OGL/RenderPasses/PassOpaqueGeometry.hpp>
#include <MSG/Renderer/OGL/RenderPasses/SubPassShadow.hpp>
#include <MSG/Renderer/OGL/RenderPasses/SubPassVTFS.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/SparseTexture.hpp>
#include <MSG/Renderer/OGL/Subsystems/FrameSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/LightsShadowSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/MeshSubsystem.hpp>

#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLPipelineInfo.hpp>
#include <MSG/OGLTypedBuffer.hpp>
#include <MSG/OGLVertexArray.hpp>

#include <MSG/Light/PunctualLight.hpp>
#include <MSG/Scene.hpp>

#include <Bindings.glsl>
#include <FrameInfo.glsl>
#include <Lights.glsl>

static inline auto GetGraphicsPipeline(
    const Msg::OGLBindings& a_GlobalBindings,
    const Msg::Renderer::Primitive& a_rPrimitive,
    const Msg::Renderer::Material& a_rMaterial,
    const Msg::Renderer::Mesh& a_rMesh,
    const Msg::Renderer::MeshSkin* a_rMeshSkin)
{
    Msg::OGLGraphicsPipelineInfo info;
    info.bindings                               = a_GlobalBindings;
    info.bindings.uniformBuffers[UBO_TRANSFORM] = { a_rMesh.transform, 0, a_rMesh.transform->size };
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
        info.bindings.textures[SAMPLERS_MATERIAL + i] = {
            textureSampler.texture,
            textureSampler.sampler,
        };
    }
    return std::move(info);
}

static inline auto GetDrawCmd(const Msg::Renderer::Primitive& a_rPrimitive)
{
    Msg::OGLCmdDrawInfo drawCmd;
    if (a_rPrimitive.vertexArray->indexed) {
        drawCmd.indexed        = true;
        drawCmd.instanceCount  = 1;
        drawCmd.instanceOffset = 0;
        drawCmd.vertexOffset   = 0;
        // indexed specific info
        drawCmd.indexCount  = a_rPrimitive.vertexArray->indexCount;
        drawCmd.indexOffset = 0;
    } else {
        drawCmd.indexed        = false;
        drawCmd.instanceCount  = 1;
        drawCmd.instanceOffset = 0;
        drawCmd.vertexOffset   = 0;
        // non indexed specific info
        drawCmd.vertexCount = a_rPrimitive.vertexArray->vertexCount;
    }
    return drawCmd;
}

Msg::Renderer::SubPassShadow::SubPassShadow(Renderer::Impl& a_Renderer)
    : RenderSubPassInterface({ typeid(SubPassVTFS) })
    , _cmdBuffer(a_Renderer.context, OGLCmdBufferType::OneShot)
    , _renderCmdBuffer(a_Renderer.context, OGLCmdBufferType::OneShot)
{
}

void Msg::Renderer::SubPassShadow::Update(Renderer::Impl& a_Renderer, RenderPassInterface* a_ParentPass)
{
    auto& subsystems      = a_Renderer.subsystemsLibrary;
    auto& shadowSubsystem = subsystems.Get<LightsShadowSubsystem>();
    _render               = shadowSubsystem.countCasters > 0;
    if (!_render)
        return;
    geometryFB = a_Renderer.renderPassesLibrary.Get<PassOpaqueGeometry>().output;
    // render shadows
    auto& activeScene    = *a_Renderer.activeScene;
    auto& registry       = *activeScene.GetRegistry();
    auto& visibleLights  = activeScene.GetVisibleEntities().lights;
    auto& frameSubsystem = subsystems.Get<FrameSubsystem>();
    _executionFence.Wait();
    _executionFence.Reset();
    _cmdBuffer.Reset();
    _cmdBuffer.Begin();
    // cmdBuffer.PushCmd<OGLCmdMemoryBarrier>(GL_BUFFER_UPDATE_BARRIER_BIT);
    uint32_t casterIndex = 0;
    for (auto& visibleLight : visibleLights) {
        auto entityRef = registry.GetEntityRef(visibleLight);
        if (!entityRef.HasComponent<LightShadowData>())
            continue;
        auto& shadowCaster     = shadowSubsystem.bufferCasters->Get(casterIndex);
        auto& shadowViewport   = shadowSubsystem.bufferViewports->Get(shadowCaster.viewportIndex);
        auto& shadowData       = entityRef.GetComponent<LightShadowData>();
        auto& punctualLight    = entityRef.GetComponent<PunctualLight>();
        const bool isCube      = shadowCaster.lightType == LIGHT_TYPE_POINT;
        shadowData.needsUpdate = true;
        OGLRenderPassInfo info;
        info.name                          = "Shadow_" + std::to_string(casterIndex);
        info.viewportState.viewportExtent  = shadowData.frameBuffer->info.defaultSize;
        info.viewportState.scissorExtent   = shadowData.frameBuffer->info.defaultSize;
        info.frameBufferState.framebuffer  = shadowData.frameBuffer;
        info.frameBufferState.drawBuffers  = { GL_COLOR_ATTACHMENT0 };
        info.frameBufferState.clear.colors = { { .index = 0, .color = { -1.f, -1.f } } };
        info.frameBufferState.clear.depth  = 1.f;
        _cmdBuffer.PushCmd<OGLCmdPushRenderPass>(info);
        OGLBindings globalBindings;
        globalBindings.uniformBuffers[UBO_FRAME_INFO] = OGLBufferBindingInfo {
            .buffer = frameSubsystem.buffer,
            .offset = 0,
            .size   = frameSubsystem.buffer->size
        };
        globalBindings.storageBuffers[SSBO_SHADOW_CASTERS] = OGLBufferBindingInfo {
            .buffer = shadowSubsystem.bufferCasters,
            .offset = uint32_t(shadowSubsystem.bufferCasters->value_size) * casterIndex,
            .size   = uint32_t(shadowSubsystem.bufferCasters->value_size)
        };
        globalBindings.storageBuffers[SSBO_SHADOW_VIEWPORTS] = OGLBufferBindingInfo {
            .buffer = shadowSubsystem.bufferViewports,
            .offset = uint32_t(shadowSubsystem.bufferViewports->value_size) * shadowCaster.viewportIndex,
            .size   = uint32_t(shadowSubsystem.bufferViewports->value_size) * shadowCaster.viewportCount
        };
        globalBindings.storageBuffers[SSBO_SHADOW_DEPTH_RANGE] = OGLBufferBindingInfo {
            .buffer = shadowData.bufferDepthRange,
            .offset = 0,
            .size   = shadowData.bufferDepthRange->size
        };
        for (auto& entity : visibleLight.meshes) {
            auto& rMaterials  = registry.GetComponent<Renderer::MaterialSet>(entity);
            auto& rMesh       = registry.GetComponent<Renderer::Mesh>(entity);
            auto rMeshSkin    = registry.HasComponent<Renderer::MeshSkin>(entity) ? &registry.GetComponent<Renderer::MeshSkin>(entity) : nullptr;
            auto& sgMaterials = registry.GetComponent<Msg::MaterialSet>(entity);
            for (auto& [rPrimitive, mtlIndex] : rMesh.at(entity.lod)) {
                auto& rMaterial        = rMaterials[mtlIndex];
                const bool isMetRough  = rMaterial->type == MATERIAL_TYPE_METALLIC_ROUGHNESS;
                const bool isSpecGloss = rMaterial->type == MATERIAL_TYPE_SPECULAR_GLOSSINESS;
                ShaderLibrary::ProgramKeywords keywords(2);
                if (isMetRough)
                    keywords[0] = { "MATERIAL_TYPE", "MATERIAL_TYPE_METALLIC_ROUGHNESS" };
                else if (isSpecGloss)
                    keywords[0] = { "MATERIAL_TYPE", "MATERIAL_TYPE_SPECULAR_GLOSSINESS" };
                keywords[1]  = { "SHADOW_CUBE", isCube ? "1" : "0" };
                auto& shader = *a_Renderer.shaderCache["Shadow"][keywords[0].second][keywords[1].second];
                if (!shader)
                    shader = a_Renderer.shaderCompiler.CompileProgram("Shadow", keywords);
                auto gpInfo                                       = GetGraphicsPipeline(globalBindings, *rPrimitive, *rMaterial, rMesh, rMeshSkin);
                gpInfo.shaderState.program                        = shader;
                gpInfo.rasterizationState.depthBiasEnable         = true;
                gpInfo.rasterizationState.depthBiasConstantFactor = punctualLight.GetShadowSettings().bias * 1000.f;
                gpInfo.rasterizationState.depthBiasSlopeFactor    = 1.5f;
                gpInfo.rasterizationState.depthBiasClamp          = punctualLight.GetShadowSettings().bias;
                _cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
                _cmdBuffer.PushCmd<OGLCmdDraw>(GetDrawCmd(*rPrimitive));
            }
        }
        _cmdBuffer.PushCmd<OGLCmdEndRenderPass>();
        casterIndex++;
    }
    _cmdBuffer.End();
    _cmdBuffer.Execute(&_executionFence);
    /// record the render command buffer
    auto& meshSubsystem = a_Renderer.subsystemsLibrary.Get<MeshSubsystem>();
    OGLCmdDrawInfo drawCmd;
    drawCmd.vertexCount = 3;
    OGLGraphicsPipelineInfo gpInfo;
    gpInfo.bindlessTextureSamplers.insert_range(gpInfo.bindlessTextureSamplers.end(), shadowSubsystem.textureSamplers);
    gpInfo.inputAssemblyState = { .primitiveTopology = GL_TRIANGLES };
    gpInfo.rasterizationState = { .cullMode = GL_NONE };
    gpInfo.vertexInputState   = { .vertexCount = 3, .vertexArray = a_Renderer.presentVAO };
    gpInfo.bindings           = meshSubsystem.globalBindings;
    gpInfo.bindings.images[0] = { geometryFB->info.colorBuffers[OUTPUT_FRAG_GBUFFER0].texture, GL_READ_WRITE, GL_RGBA32UI };
    gpInfo.bindings.images[1] = { geometryFB->info.colorBuffers[OUTPUT_FRAG_GBUFFER1].texture, GL_READ_WRITE, GL_RGBA32UI };
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
    _renderCmdBuffer.Reset();
    _renderCmdBuffer.Begin();
    _renderCmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
    _renderCmdBuffer.PushCmd<OGLCmdDraw>(drawCmd);
    _renderCmdBuffer.End();
}

void Msg::Renderer::SubPassShadow::UpdateSettings(Renderer::Impl& a_Renderer, const RendererSettings& a_Settings)
{
    const ShaderLibrary::ProgramKeywords keywords = { { "SHADOW_QUALITY", std::to_string(int(a_Settings.shadowQuality) + 1) } };
    shader                                        = *a_Renderer.shaderCache["DeferredShadows"][keywords[0].second];
    if (shader == nullptr)
        shader = a_Renderer.shaderCompiler.CompileProgram("DeferredShadows", keywords);
}

void Msg::Renderer::SubPassShadow::Render(Impl& a_Renderer)
{
    if (_render)
        a_Renderer.renderCmdBuffer.PushCmd<OGLCmdPushCmdBuffer>(_renderCmdBuffer);
}
