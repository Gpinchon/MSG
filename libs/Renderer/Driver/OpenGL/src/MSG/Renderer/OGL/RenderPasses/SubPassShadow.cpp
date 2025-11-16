#include <MSG/Renderer/OGL/Components/LightData.hpp>
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
#include <MSG/Renderer/OGL/Subsystems/LightsSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/MeshSubsystem.hpp>

#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLPipelineInfo.hpp>
#include <MSG/OGLTypedBuffer.hpp>
#include <MSG/OGLVertexArray.hpp>

#include <MSG/Scene.hpp>

#include <Bindings.glsl>
#include <FrameInfo.glsl>
#include <LightsShadowInputs.glsl>

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
    return info;
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
    , cmdBuffer(a_Renderer.context, OGLCmdBufferType::OneShot)
{
}

void Msg::Renderer::SubPassShadow::Update(Renderer::Impl& a_Renderer, RenderPassInterface* a_ParentPass)
{
    geometryFB = a_Renderer.renderPassesLibrary.Get<PassOpaqueGeometry>().output;
    // render shadows
    auto& cmdBuffer      = a_Renderer.renderCmdBuffer;
    auto& subsystems     = a_Renderer.subsystemsLibrary;
    auto& activeScene    = *a_Renderer.activeScene;
    auto& registry       = *activeScene.GetRegistry();
    auto& frameSubsystem = subsystems.Get<FrameSubsystem>();
    auto& lightSubsystem = subsystems.Get<LightsSubsystem>();
    auto& shadows        = lightSubsystem.shadows;
    if (shadows.dataBuffer->Get().count == 0)
        return;
    executionFence.Wait();
    executionFence.Reset();
    cmdBuffer.Reset();
    cmdBuffer.Begin();
    for (uint32_t i = 0; i < shadows.dataBuffer->Get().count; i++) {
        auto& glslData      = shadows.dataBuffer->Get().shadows[i];
        auto& visibleShadow = activeScene.GetVisibleEntities().shadows[i];
        auto& lightData     = registry.GetComponent<Renderer::LightData>(visibleShadow);
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
            cmdBuffer.PushCmd<OGLCmdPushRenderPass>(info);
            OGLBindings globalBindings;
            globalBindings.uniformBuffers[UBO_FRAME_INFO] = OGLBufferBindingInfo {
                .buffer = frameSubsystem.buffer,
                .offset = 0,
                .size   = frameSubsystem.buffer->size
            };
            globalBindings.storageBuffers[SSBO_SHADOW_DATA] = OGLBufferBindingInfo {
                .buffer = shadows.dataBuffer,
                .offset = uint32_t(offsetof(GLSL::ShadowsBase, shadows) + sizeof(GLSL::ShadowBase) * i),
                .size   = sizeof(GLSL::ShadowBase)
            };
            globalBindings.storageBuffers[SSBO_SHADOW_VIEWPORTS] = OGLBufferBindingInfo {
                .buffer = shadows.viewportsBuffer,
                .offset = uint32_t(sizeof(GLSL::Camera) * (glslData.viewportIndex + vI)),
                .size   = sizeof(GLSL::Camera)
            };
            globalBindings.storageBuffers[SSBO_SHADOW_DEPTH_RANGE] = OGLBufferBindingInfo {
                .buffer = shadowData.depthRanges[shadowData.depthRangeIndex],
                .offset = 0,
                .size   = shadowData.depthRanges[shadowData.depthRangeIndex]->size
            };
            globalBindings.storageBuffers[SSBO_SHADOW_DEPTH_RANGE + 1] = OGLBufferBindingInfo {
                .buffer = shadowData.depthRanges[shadowData.depthRangeIndex_Prev],
                .offset = 0,
                .size   = shadowData.depthRanges[shadowData.depthRangeIndex_Prev]->size
            };
            for (auto& entity : viewPort.meshes) {
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
                    auto gpInfo                = GetGraphicsPipeline(globalBindings, *rPrimitive, *rMaterial, rMesh, rMeshSkin);
                    gpInfo.shaderState.program = shader;
                    cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
                    cmdBuffer.PushCmd<OGLCmdDraw>(GetDrawCmd(*rPrimitive));
                }
            }
            cmdBuffer.PushCmd<OGLCmdEndRenderPass>();
        }
    }
    cmdBuffer.End();
    cmdBuffer.Execute(&executionFence);
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
    executionFence.Wait();
    auto& meshSubsystem = a_Renderer.subsystemsLibrary.Get<MeshSubsystem>();
    auto& cmdBuffer     = a_Renderer.renderCmdBuffer;
    OGLCmdDrawInfo drawCmd;
    drawCmd.vertexCount = 3;
    OGLGraphicsPipelineInfo gpInfo;
    gpInfo.inputAssemblyState = { .primitiveTopology = GL_TRIANGLES };
    gpInfo.rasterizationState = { .cullMode = GL_NONE };
    gpInfo.vertexInputState   = { .vertexCount = 3, .vertexArray = a_Renderer.presentVAO };
    gpInfo.bindings           = meshSubsystem.globalBindings;
    gpInfo.bindings.images[0] = { geometryFB->info.colorBuffers[OUTPUT_FRAG_DFD_GBUFFER0].texture, GL_READ_WRITE, GL_RGBA32UI };
    gpInfo.bindings.images[1] = { geometryFB->info.colorBuffers[OUTPUT_FRAG_DFD_GBUFFER1].texture, GL_READ_WRITE, GL_RGBA32UI };
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