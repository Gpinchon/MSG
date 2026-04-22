#include <MSG/Renderer/OGL/Components/LightShadowData.hpp>
#include <MSG/Renderer/OGL/Components/MaterialSet.hpp>
#include <MSG/Renderer/OGL/Components/Mesh.hpp>
#include <MSG/Renderer/OGL/Components/MeshSkin.hpp>
#include <MSG/Renderer/OGL/Material.hpp>
#include <MSG/Renderer/OGL/Primitive.hpp>
#include <MSG/Renderer/OGL/RenderPasses/PassShadowMaps.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/Subsystems/FrameSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/LightsShadowSubsystem.hpp>
#include <MSG/Renderer/OGL/VirtualTexture.hpp>

#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLPipelineInfo.hpp>
#include <MSG/OGLRenderPassInfo.hpp>
#include <MSG/OGLTypedBuffer.hpp>
#include <MSG/OGLVertexArray.hpp>

#include <MSG/Light/PunctualLight.hpp>
#include <MSG/Scene.hpp>

#include <Bindings.glsl>
#include <FrameInfo.glsl>
#include <Lights.glsl>

static inline auto GetGraphicsPipeline(
    Msg::Renderer::Impl& a_Rdr,
    const Msg::OGLBindings& a_GlobalBindings,
    const std::shared_ptr<Msg::OGLTexture>& a_Atlas,
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
    info.bindings.textures[SAMPLERS_MATERIAL_ATLAS] = { a_Atlas, nullptr };
    for (uint32_t i = 0; i < SAMPLERS_MATERIAL_COUNT; ++i) {
        info.bindings.textures[SAMPLERS_MATERIAL_PAGE_TABLE + i] = {
            a_rMaterial.pageTables[i],
            Msg::Renderer::Material::GetPageTableSampler(a_Rdr)
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

Msg::Renderer::PassShadowMaps::PassShadowMaps(Renderer::Impl& a_Rdr)
    : RenderPassInterface()
    , _cmdBuffer(a_Rdr.context, OGLCmdBufferType::OneShot)
{
}

void Msg::Renderer::PassShadowMaps::Update(Renderer::Impl& a_Rdr, const RenderPassesLibrary& a_RenderPasses)
{
    auto& subsystems      = a_Rdr.subsystemsLibrary;
    auto& shadowSubsystem = subsystems.Get<LightsShadowSubsystem>();
    _render               = shadowSubsystem.countCasters > 0;
    if (!_render)
        return;
    // render shadows
    const auto& atlas    = a_Rdr.vtLoader.GetAtlas();
    auto& activeScene    = *a_Rdr.activeScene;
    auto& registry       = *activeScene.GetRegistry();
    auto& visibleLights  = activeScene.GetVisibleEntities().lights;
    auto& frameSubsystem = subsystems.Get<FrameSubsystem>();
    _cmdBuffer.Reset();
    _cmdBuffer.Begin();
    uint32_t casterIndex = 0;
    for (auto& visibleLight : visibleLights) {
        auto entityRef = registry.GetEntityRef(visibleLight);
        if (!entityRef.HasComponent<LightShadowData>())
            continue;
        auto& shadowCaster     = shadowSubsystem.bufferCasters->Get(casterIndex);
        auto& shadowViewport   = shadowSubsystem.bufferViewports->Get(shadowCaster.viewportIndex);
        auto& shadowData       = entityRef.GetComponent<LightShadowData>();
        auto& punctualLight    = entityRef.GetComponent<PunctualLight>();
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
            auto& rMaterials = registry.GetComponent<Renderer::MaterialSet>(entity);
            auto& rMesh      = registry.GetComponent<Renderer::Mesh>(entity);
            auto rMeshSkin   = registry.HasComponent<Renderer::MeshSkin>(entity) ? &registry.GetComponent<Renderer::MeshSkin>(entity) : nullptr;
            for (auto& [rPrimitive, mtlIndex] : rMesh.at(entity.lod)) {
                auto& rMaterial = rMaterials[mtlIndex];
                auto shader     = a_Rdr.shaderCompiler.CompileProgram("Shadow", // order is important
                    ShaderLibrary::ProgramKeyword { TO_STRING(SKINNED), rMeshSkin != nullptr ? "1" : "0" },
                    ShaderLibrary::ProgramKeyword { TO_STRING(MATERIAL_TYPE), GLSL::MaterialTypeToString(rMaterial->type) },
                    ShaderLibrary::ProgramKeyword { TO_STRING(MATERIAL_ALPHA_MODE), GLSL::MaterialAlphaModeToString(rMaterial->alphaMode) },
                    ShaderLibrary::ProgramKeyword { TO_STRING(LIGHT_TYPE), GLSL::LightTypeToString(shadowCaster.lightType) });

                OGLGraphicsPipelineInfo gpInfo                    = GetGraphicsPipeline(a_Rdr, globalBindings, atlas, *rPrimitive, *rMaterial, rMesh, rMeshSkin);
                gpInfo.shaderState.program                        = shader;
                gpInfo.depthStencilState.depthCompareOp           = GL_LEQUAL;
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
}

void Msg::Renderer::PassShadowMaps::Render(Impl& a_Rdr)
{
    if (_render)
        a_Rdr.renderCmdBuffer.PushCmd<OGLCmdPushCmdBuffer>(_cmdBuffer);
}
