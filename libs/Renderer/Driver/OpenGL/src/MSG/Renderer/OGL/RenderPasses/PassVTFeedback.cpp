#include <MSG/Renderer/OGL/Components/MaterialSet.hpp>
#include <MSG/Renderer/OGL/Components/Mesh.hpp>
#include <MSG/Renderer/OGL/Components/MeshSkin.hpp>
#include <MSG/Renderer/OGL/Material.hpp>
#include <MSG/Renderer/OGL/Primitive.hpp>
#include <MSG/Renderer/OGL/RenderBuffer.hpp>
#include <MSG/Renderer/OGL/RenderPasses/PassVTFeedback.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/Subsystems/CameraSubsystem.hpp>
#include <MSG/Renderer/OGL/VirtualTexture.hpp>

#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLPipelineInfo.hpp>
#include <MSG/OGLTexture.hpp>
#include <MSG/OGLTexture2D.hpp>
#include <MSG/OGLTexture2DArray.hpp>
#include <MSG/OGLTypedBuffer.hpp>
#include <MSG/OGLVertexArray.hpp>

#include <MSG/Mesh.hpp>
#include <MSG/Scene.hpp>

#include <GL/glew.h>

#include <future>
#include <unordered_set>

#include <VirtualTexturing.glsl>

#include <glm/gtc/packing.hpp>

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

static inline auto GetFeedbackBindings(
    const std::shared_ptr<Msg::OGLTypedBuffer<Msg::Renderer::GLSL::CameraUBO>>& a_CamBuffer,
    const std::shared_ptr<Msg::OGLTypedBuffer<Msg::Renderer::GLSL::VTFeedbackSettings>>& a_SettingsBuffer,
    const std::shared_ptr<Msg::OGLTypedBufferArray<Msg::Renderer::GLSL::VTFeedbackMaterialInfo>>& a_MtlBuffer,
    const uint32_t& a_MtlIndex)
{
    uint32_t mtlOffset = sizeof(Msg::Renderer::GLSL::VTFeedbackMaterialInfo) * a_MtlIndex;
    Msg::OGLBindings bindings;
    bindings.uniformBuffers[UBO_CAMERA]      = { a_CamBuffer, 0, a_CamBuffer->size };
    bindings.uniformBuffers[UBO_VT_SETTINGS] = { a_SettingsBuffer, 0, a_SettingsBuffer->size };
    bindings.storageBuffers[0]               = { a_MtlBuffer, mtlOffset, sizeof(Msg::Renderer::GLSL::VTFeedbackMaterialInfo) };
    return bindings;
}

static inline auto GetGraphicsPipeline(
    Msg::Renderer::Impl& a_Rdr,
    const Msg::OGLBindings& a_GlobalBindings,
    const std::shared_ptr<Msg::OGLVertexArray>& a_VAO,
    const std::shared_ptr<Msg::OGLTexture>& a_Atlas,
    const Msg::Renderer::Primitive& a_rPrimitive,
    const Msg::Renderer::Material& a_rMaterial,
    const Msg::Renderer::Mesh& a_rMesh,
    const Msg::Renderer::MeshSkin* a_rMeshSkin)
{
    Msg::OGLGraphicsPipelineInfo info;
    info.bindings                                   = a_GlobalBindings;
    info.bindings.uniformBuffers[UBO_TRANSFORM]     = { a_rMesh.transform, 0, a_rMesh.transform->size };
    info.bindings.uniformBuffers[UBO_MATERIAL]      = { a_rMaterial.buffer, 0, a_rMaterial.buffer->size };
    info.bindings.textures[SAMPLERS_MATERIAL_ATLAS] = { a_Atlas, nullptr };
    for (uint32_t i = 0; i < SAMPLERS_MATERIAL_COUNT; ++i) {
        info.bindings.textures[SAMPLERS_MATERIAL_PAGE_TABLE + i] = {
            a_rMaterial.pageTables[i],
            Msg::Renderer::Material::GetPageTableSampler(a_Rdr)
        };
    }
    info.inputAssemblyState.primitiveTopology = a_rPrimitive.drawMode;
    info.vertexInputState.vertexArray         = a_VAO;
    // info.vertexInputState.vertexArray           = a_rPrimitive.vertexArray;
    info.rasterizationState.cullMode = a_rMaterial.doubleSided ? GL_NONE : GL_BACK;
    if (a_rMeshSkin != nullptr) [[unlikely]] {
        info.bindings.storageBuffers[SSBO_MESH_SKIN]      = { a_rMeshSkin->buffer, 0, a_rMeshSkin->buffer->size };
        info.bindings.storageBuffers[SSBO_MESH_SKIN_PREV] = { a_rMeshSkin->buffer_Previous, 0, a_rMeshSkin->buffer_Previous->size };
    }
    return info;
}

Msg::OGLContextCreateInfo GetFeedbackCtxInfo(Msg::OGLContext& a_RdrCtx)
{
    return Msg::OGLContextCreateInfo {
        .sharedContext  = &a_RdrCtx,
        .setPixelFormat = true
    };
}

Msg::Renderer::PassVTFeedback::PassVTFeedback(Renderer::Impl& a_Rdr)
    : _ctx(CreateHeadlessOGLContext(GetFeedbackCtxInfo(a_Rdr.context)))
    , _feedbackSettingsBuffer(std::make_shared<OGLTypedBuffer<GLSL::VTFeedbackSettings>>(a_Rdr.context))
    , _feedbackProgram(a_Rdr.shaderCompiler.CompileProgram("VTFeedback", ShaderLibrary::ProgramKeywords { { "SKINNED", "0" } }))
    , _feedbackProgramSkinned(a_Rdr.shaderCompiler.CompileProgram("VTFeedback", ShaderLibrary::ProgramKeywords { { "SKINNED", "1" } }))
    , _feedbackCmdBuffer(_ctx, OGLCmdBufferType::OneShot)
{
}

void Msg::Renderer::PassVTFeedback::Update(Renderer::Impl& a_Rdr, const RenderPassesLibrary& a_RenderPasses)
{
    auto& activeRenderBuffer = *a_Rdr.activeRenderBuffer;
    glm::uvec2 bufferRes     = glm::vec2 { activeRenderBuffer->width, activeRenderBuffer->height } * a_Rdr.settings.internalResolution;
    _CreateFeedbackBuffers(bufferRes);
    // we have a feedback pass pending
    if (_feedbackFence.WaitFor(1)) {
        if (_feedbackRequested)
            _feedbackFB->info.colorBuffers[0].texture->DownloadLevel(0,
                GL_RGB_INTEGER, GL_UNSIGNED_INT,
                _feedbackTexBuffer.size() * sizeof(_feedbackTexBuffer.front()), _feedbackTexBuffer.data());
        _feedbackReady     = true;
        _feedbackRequested = false;
    }
    auto vaoItr = _VAOs.begin();
    while (vaoItr != _VAOs.end()) {
        if (vaoItr->first.use_count() == 1)
            vaoItr = _VAOs.erase(vaoItr); // we're the last ones keeping this ref alive
        vaoItr++;
    }
}

void Msg::Renderer::PassVTFeedback::Render(Impl& a_Rdr)
{
    // uncomment this to make Nsight capture easier
    // using namespace std::chrono_literals;
    // std::this_thread::sleep_for(35ms);
    const auto now         = std::chrono::system_clock::now();
    const auto elapsedTime = now - _lastUpdate;
    const auto& atlas      = a_Rdr.sparseTextureLoader.GetAtlas();
    const auto& camBuffer  = a_Rdr.subsystemsLibrary.Get<Msg::Renderer::CameraSubsystem>().buffer;
    if (elapsedTime >= VTPollingRate && _feedbackFence.WaitFor(1)) {
        _lastUpdate           = now;
        auto& activeScene     = *a_Rdr.activeScene;
        auto& registry        = *activeScene.GetRegistry();
        auto& visibleEntities = activeScene.GetVisibleEntities();
        // create a new feedback pass
        std::unordered_map<const Material*, uint32_t> materialsID;
        std::vector<GLSL::VTFeedbackMaterialInfo> materials;
        materialsID.reserve(1024);
        materials.reserve(1024);
        for (auto& entity : visibleEntities.meshes) {
            auto& rMaterials = registry.GetComponent<Renderer::MaterialSet>(entity);
            auto& rMesh      = registry.GetComponent<Renderer::Mesh>(entity);
            for (auto& [rPrimitive, mtlIndex] : rMesh.at(entity.lod)) {
                auto& rMaterial = rMaterials[mtlIndex];
                auto mtlIDItr   = materialsID.find(rMaterial.get());
                if (mtlIDItr != materialsID.end())
                    continue; // material already processed
                else
                    mtlIDItr = materialsID.insert({ rMaterial.get(), uint32_t(materials.size()) }).first;
                auto& mtlInfo = materials.emplace_back();
                auto& glslMat = rMaterial->buffer->Get();
                for (uint32_t i = 0; i < SAMPLERS_MATERIAL_COUNT; i++) {
                    mtlInfo.textures[i].info = rMaterial->buffer->Get().textureInfos[i];
                    mtlInfo.textures[i].id   = glm::unpackUint2x32(reinterpret_cast<uintptr_t>(rMaterial->textures[i].get()));
                }
            }
        }
        if (!materials.empty()) {
            if (_feedbackMaterialsBuffer == nullptr || _feedbackMaterialsBuffer->GetCount() < materials.size())
                _feedbackMaterialsBuffer = std::make_shared<OGLTypedBufferArray<GLSL::VTFeedbackMaterialInfo>>(_ctx, materials.size());
            _feedbackMaterialsBuffer->Set(0, materials.size(), materials.data());
            _feedbackMaterialsBuffer->Update();
            _feedbackFence.Reset();
            _feedbackCmdBuffer.Begin();
            _feedbackCmdBuffer.PushCmd<OGLCmdPushRenderPass>(_feedbackRenderPass);
            for (auto& entity : visibleEntities.meshes) {
                auto& sgMesh    = registry.GetComponent<Msg::Mesh>(entity);
                auto& sgMeshLod = sgMesh.at(entity.lod);
                auto& rMesh     = registry.GetComponent<Renderer::Mesh>(entity);
                auto rMaterials = registry.GetComponent<Renderer::MaterialSet>(entity);
                auto rMeshSkin  = registry.HasComponent<Renderer::MeshSkin>(entity) ? &registry.GetComponent<Renderer::MeshSkin>(entity) : nullptr;
                for (auto& [rPrimitive, mtlIndex] : rMesh.at(entity.lod)) {
                    auto& rMaterial = rMaterials[mtlIndex];
                    auto mtlID      = materialsID.at(rMaterial.get());
                    auto vao        = _LoadPrimitive(*rPrimitive);
                    auto gp         = GetGraphicsPipeline(
                        a_Rdr,
                        GetFeedbackBindings(camBuffer, _feedbackSettingsBuffer, _feedbackMaterialsBuffer, mtlID),
                        vao, atlas,
                        *rPrimitive, *rMaterial,
                        rMesh, rMeshSkin);
                    gp.shaderState.program = rMeshSkin != nullptr ? _feedbackProgramSkinned : _feedbackProgram;
                    _feedbackCmdBuffer.PushCmd<OGLCmdPushPipeline>(gp);
                    _feedbackCmdBuffer.PushCmd<OGLCmdDraw>(GetDrawCmd(*rPrimitive));
                }
            }
            _feedbackCmdBuffer.PushCmd<OGLCmdEndRenderPass>();
            _feedbackCmdBuffer.PushCmd<OGLCmdMemoryBarrier>(GL_TEXTURE_UPDATE_BARRIER_BIT);
            _feedbackCmdBuffer.End();
            _feedbackCmdBuffer.Execute(&_feedbackFence);
            _feedbackReady     = false;
            _feedbackRequested = true;
        }
    }
}

bool Msg::Renderer::PassVTFeedback::GetFeedbackReady() const
{
    return _feedbackReady;
}

const std::vector<glm::uvec3>& Msg::Renderer::PassVTFeedback::GetFeedbackBuffer() const
{
    return _feedbackTexBuffer;
}

const glm::uvec3& Msg::Renderer::PassVTFeedback::GetFeedbackRes() const
{
    return _feedbackRes;
}

void Msg::Renderer::PassVTFeedback::_CreateFeedbackBuffers(const glm::uvec2& a_BufferRes)
{
    glm::uvec2 newRes = glm::max(a_BufferRes / 16u, 16u);
    if (newRes == glm::uvec2(_feedbackRes.x, _feedbackRes.y))
        return;
    _feedbackRes = glm::uvec3(newRes, SAMPLERS_MATERIAL_COUNT);
    OGLFrameBufferCreateInfo feedbackFBInfo;
    feedbackFBInfo.defaultSize = _feedbackRes;
    feedbackFBInfo.colorBuffers.resize(1);
    feedbackFBInfo.colorBuffers[0].attachment = GL_COLOR_ATTACHMENT0;
    feedbackFBInfo.colorBuffers[0].layer      = 0;
    feedbackFBInfo.colorBuffers[0].texture    = std::make_shared<OGLTexture2DArray>(_ctx,
        OGLTexture2DArrayInfo {
            .width       = _feedbackRes.x,
            .height      = _feedbackRes.y,
            .layers      = _feedbackRes.z,
            .sizedFormat = GL_RGB32UI });
    feedbackFBInfo.depthBuffer.texture        = std::make_shared<OGLTexture2DArray>(_ctx,
        OGLTexture2DArrayInfo {
            .width       = _feedbackRes.x,
            .height      = _feedbackRes.y,
            .layers      = _feedbackRes.z,
            .sizedFormat = GL_DEPTH_COMPONENT16,
        });
    feedbackFBInfo.colorBuffers[0].texture->Clear(GL_RGB, GL_UNSIGNED_INT, 0, &glm::uvec3(0).data);
    _feedbackFB = std::make_shared<OGLFrameBuffer>(_ctx, feedbackFBInfo);
    _feedbackTexBuffer.resize(_feedbackRes.x * _feedbackRes.y * _feedbackRes.z);
    GLSL::VTFeedbackSettings settings = _feedbackSettingsBuffer->Get();
    settings.bufferRatio              = glm::vec2(_feedbackRes.x, _feedbackRes.y) / glm::vec2(a_BufferRes);
    _feedbackSettingsBuffer->Set(settings);
    _feedbackSettingsBuffer->Update();
    _feedbackRenderPass.name = "VTFeedback";
    _feedbackRenderPass.frameBufferState.clear.colors.resize(1);
    _feedbackRenderPass.frameBufferState.clear.colors[0] = { .index = 0, .color = { 0, 0, 0 } };
    _feedbackRenderPass.frameBufferState.clear.depth     = 1.f;
    _feedbackRenderPass.frameBufferState.drawBuffers     = { GL_COLOR_ATTACHMENT0 };
    _feedbackRenderPass.frameBufferState.framebuffer     = _feedbackFB;
    _feedbackRenderPass.viewportState.viewportExtent     = _feedbackFB->info.defaultSize;
    _feedbackRenderPass.viewportState.scissorExtent      = _feedbackFB->info.defaultSize;
}

std::shared_ptr<Msg::OGLVertexArray> Msg::Renderer::PassVTFeedback::_LoadPrimitive(Renderer::Primitive& a_rPrimitive)
{
    auto vaoItr = _VAOs.find(a_rPrimitive.vertexArray);
    if (vaoItr == _VAOs.end()) {
        std::shared_ptr<OGLVertexArray> vao;
        if (a_rPrimitive.vertexArray->indexed)
            vao = std::make_shared<Msg::OGLVertexArray>(_ctx,
                a_rPrimitive.vertexArray->vertexCount,
                a_rPrimitive.vertexArray->attributesDesc,
                a_rPrimitive.vertexArray->vertexBindings,
                a_rPrimitive.vertexArray->indexCount,
                a_rPrimitive.vertexArray->indexDesc,
                a_rPrimitive.vertexArray->indexBuffer);
        else
            vao = std::make_shared<Msg::OGLVertexArray>(_ctx,
                a_rPrimitive.vertexArray->vertexCount,
                a_rPrimitive.vertexArray->attributesDesc,
                a_rPrimitive.vertexArray->vertexBindings);
        vaoItr = _VAOs.insert({ a_rPrimitive.vertexArray, vao }).first;
    }
    return vaoItr->second;
}