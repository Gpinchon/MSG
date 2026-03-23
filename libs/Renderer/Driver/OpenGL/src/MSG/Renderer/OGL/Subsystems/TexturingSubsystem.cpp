#include <MSG/Renderer/OGL/Material.hpp>
#include <MSG/Renderer/OGL/Primitive.hpp>
#include <MSG/Renderer/OGL/RenderBuffer.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/VirtualTexture.hpp>

#include <MSG/Renderer/OGL/Components/Mesh.hpp>
#include <MSG/Renderer/OGL/Components/MeshSkin.hpp>

#include <MSG/Renderer/OGL/Subsystems/CameraSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/MaterialSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/MeshSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/TexturingSubsystem.hpp>

#include <MSG/ECS/Registry.hpp>
#include <MSG/MaterialSet.hpp>
#include <MSG/Mesh.hpp>
#include <MSG/Scene.hpp>
#include <MSG/Texture.hpp>

#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLRenderPassInfo.hpp>
#include <MSG/OGLSampler.hpp>
#include <MSG/OGLTexture2D.hpp>
#include <MSG/OGLTexture2DArray.hpp>
#include <MSG/OGLTypedBuffer.hpp>
#include <MSG/OGLVertexArray.hpp>

#include <span>

#include <glm/gtc/packing.hpp>

static inline auto GetFeedbackBindings(
    const Msg::Renderer::SubsystemsLibrary& a_Subsystems,
    const uint32_t& a_MtlIndex)
{
    auto& cameraSubsystem    = a_Subsystems.Get<Msg::Renderer::CameraSubsystem>();
    auto& texturingSubsystem = a_Subsystems.Get<Msg::Renderer::TexturingSubsystem>();
    auto& mtlBuffer          = texturingSubsystem.feedbackMaterialsBuffer;
    auto& settingsBuffer     = texturingSubsystem.feedbackSettingsBuffer;
    uint32_t mtlOffset       = sizeof(Msg::Renderer::GLSL::VTFeedbackMaterialInfo) * a_MtlIndex;
    Msg::OGLBindings bindings;
    bindings.uniformBuffers[UBO_CAMERA]      = { cameraSubsystem.buffer, 0, cameraSubsystem.buffer->size };
    bindings.uniformBuffers[UBO_VT_SETTINGS] = { settingsBuffer, 0, settingsBuffer->size };
    bindings.storageBuffers[0]               = { mtlBuffer, mtlOffset, sizeof(Msg::Renderer::GLSL::VTFeedbackMaterialInfo) };
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

Msg::OGLFrameBufferCreateInfo GetFeedbackFBInfo(const glm::uvec3& a_Res)
{
    return Msg::OGLFrameBufferCreateInfo {
        .defaultSize = a_Res
    };
}

Msg::OGLContextCreateInfo GetFeedbackCtxInfo(Msg::OGLContext& a_RdrCtx)
{
    return Msg::OGLContextCreateInfo {
        .sharedContext  = &a_RdrCtx,
        .setPixelFormat = true
    };
}

Msg::Renderer::TexturingSubsystem::TexturingSubsystem(Renderer::Impl& a_Renderer)
    : SubsystemInterface({
          typeid(CameraSubsystem),
          typeid(MaterialSubsystem),
          typeid(MeshSubsystem),
      })
    , ctx(CreateHeadlessOGLContext(GetFeedbackCtxInfo(a_Renderer.context)))
    , feedbackSettingsBuffer(std::make_shared<OGLTypedBuffer<GLSL::VTFeedbackSettings>>(a_Renderer.context))
    , _feedbackProgram(a_Renderer.shaderCompiler.CompileProgram("VTFeedback", ShaderLibrary::ProgramKeywords { { "SKINNED", "0" } }))
    , _feedbackProgramSkinned(a_Renderer.shaderCompiler.CompileProgram("VTFeedback", ShaderLibrary::ProgramKeywords { { "SKINNED", "1" } }))
    , _feedbackFence(true)
    , _feedbackCmdBuffer(ctx, OGLCmdBufferType::OneShot)
{
}

void Msg::Renderer::TexturingSubsystem::Load(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
}

void Msg::Renderer::TexturingSubsystem::Unload(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
}

uint32_t GetVTWrapMode(const uint32_t& a_WrapMode)
{
    switch (a_WrapMode) {
    case GL_REPEAT:
        return VT_WRAP_REPEAT;
    case GL_MIRRORED_REPEAT:
        return VT_WRAP_REPEAT_MIRROR;
    case GL_CLAMP:
    case GL_CLAMP_TO_EDGE:
    case GL_CLAMP_TO_BORDER:
        return VT_WRAP_CLAMP;
    case GL_MIRROR_CLAMP_TO_EDGE:
        return VT_WRAP_CLAMP_MIRROR;
    default:
        break;
    }
    return GL_NONE;
}

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::milliseconds ms;
typedef std::chrono::duration<float> fsec;

void Msg::Renderer::TexturingSubsystem::Update(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems)
{
    auto& activeRenderBuffer = *a_Renderer.activeRenderBuffer;
    glm::uvec2 bufferRes     = glm::vec2 { activeRenderBuffer->width, activeRenderBuffer->height } * a_Renderer.settings.internalResolution;
    _FetchUsedPages();
    _UploadPages(a_Renderer);
    _CreateFeedbackBuffers(bufferRes);
    _PollUsedPages(a_Renderer, a_Subsystems);
    auto vaoItr = _VAOs.begin();
    while (vaoItr != _VAOs.end()) {
        if (vaoItr->first.use_count() == 1)
            vaoItr = _VAOs.erase(vaoItr); // we're the last ones keeping this ref alive
        vaoItr++;
    }
}

void Msg::Renderer::TexturingSubsystem::_FetchUsedPages()
{
    if (!_feedbackRequested)
        return;
    _feedbackFence.Wait();
    _feedbackRequested = false;
    _feedbackFB->info.colorBuffers[0].texture->DownloadLevel(0,
        GL_RGB_INTEGER, GL_UNSIGNED_INT,
        _feedbackTexBuffer.size() * sizeof(_feedbackTexBuffer.front()), _feedbackTexBuffer.data());
    // gather the used pages
    using UsedSamplerPages = std::unordered_map<VirtualTexture*, std::unordered_set<uint32_t>>;
    std::array<std::future<UsedSamplerPages>, SAMPLERS_MATERIAL_COUNT> jobs;
    for (uint8_t samplerI = 0; samplerI < _feedbackRes.z; samplerI++) {
        const uint32_t textureSize = _feedbackRes.x * _feedbackRes.y;
        auto spanBeg               = _feedbackTexBuffer.begin() + textureSize * (samplerI + 0);
        auto spanEnd               = _feedbackTexBuffer.begin() + textureSize * (samplerI + 1);
        jobs[samplerI]             = _feedbackThreadPool.Enqueue([&, feedbackSpan = std::span<glm::uvec3>(spanBeg, spanEnd)] {
            UsedSamplerPages samplerPages;
            for (auto& val : feedbackSpan) {
                auto sampler = reinterpret_cast<VirtualTexture*>(glm::packUint2x32({ val.x, val.y }));
                if (sampler == nullptr)
                    continue;
                auto uv    = glm::unpackUnorm4x8(val.z);
                auto level = uv[3] * sampler->GetLevels();
                auto itr   = samplerPages.find(sampler);
                if (itr == samplerPages.end())
                    itr = samplerPages.insert({ sampler, { } }).first;
                itr->second.insert(sampler->GetPageID(glm::vec3(uv.x, uv.y, uv.z), floor(level)));
                itr->second.insert(sampler->GetPageID(glm::vec3(uv.x, uv.y, uv.z), ceil(level)));
            }
            return samplerPages;
        });
    }
    // request necessary pages
    for (auto& samplerPages : jobs) {
        for (auto& texPage : samplerPages.get()) {
            bool anyMissing = false;
            for (auto& page : texPage.second)
                anyMissing |= texPage.first->RequestPage(page);
            if (anyMissing)
                _managedTextures.insert(texPage.first->shared_from_this());
        }
    }
}

void Msg::Renderer::TexturingSubsystem::_UploadPages(Renderer::Impl& a_Renderer)
{
    // early bail if the upload function did not run yet
    if (_pagesUploaded.load() && !_managedTextures.empty()) {
        _pagesUploaded.store(false);
        a_Renderer.context.PushCmd([this] {
            auto remainingTime = SparseTextureUploadTimeBudget;
            auto managedItr    = _managedTextures.begin();
            while (remainingTime > ms(0u) && managedItr != _managedTextures.end()) {
                auto& managedTxt = *managedItr;
                remainingTime -= managedTxt->CommitPendingPages(remainingTime);
                managedTxt->FreeUnusedPages();
                if (managedTxt->Empty())
                    managedItr = _managedTextures.erase(managedItr);
                else
                    managedItr++;
            }
            _pagesUploaded.store(true);
        });
    }
}

void Msg::Renderer::TexturingSubsystem::_PollUsedPages(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems)
{
    // uncomment this to make Nsight capture easier
    // using namespace std::chrono_literals;
    // std::this_thread::sleep_for(35ms);
    const auto now         = std::chrono::system_clock::now();
    const auto elapsedTime = now - _lastUpdate;
    const auto& atlas      = a_Renderer.sparseTextureLoader.GetAtlas();
    if (elapsedTime >= SparseTexturePollingRate) {
        _lastUpdate           = now;
        auto& activeScene     = *a_Renderer.activeScene;
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
            if (feedbackMaterialsBuffer == nullptr || feedbackMaterialsBuffer->GetCount() < materials.size())
                feedbackMaterialsBuffer = std::make_shared<OGLTypedBufferArray<GLSL::VTFeedbackMaterialInfo>>(ctx, materials.size());
            feedbackMaterialsBuffer->Set(0, materials.size(), materials.data());
            feedbackMaterialsBuffer->Update();
            OGLRenderPassInfo renderPass;
            renderPass.name = "VTFeedback";
            renderPass.frameBufferState.clear.colors.resize(1);
            renderPass.frameBufferState.clear.colors[0] = { .index = 0, .color = { 0, 0, 0 } };
            renderPass.frameBufferState.clear.depth     = 1.f;
            renderPass.frameBufferState.drawBuffers     = { GL_COLOR_ATTACHMENT0 };
            renderPass.frameBufferState.framebuffer     = _feedbackFB;
            renderPass.viewportState.viewportExtent     = _feedbackFB->info.defaultSize;
            renderPass.viewportState.scissorExtent      = _feedbackFB->info.defaultSize;
            _feedbackCmdBuffer.Begin();
            _feedbackCmdBuffer.PushCmd<OGLCmdPushRenderPass>(renderPass);
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
                        a_Renderer,
                        GetFeedbackBindings(a_Subsystems, mtlID),
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
            _feedbackFence.Reset();
            _feedbackCmdBuffer.Execute(&_feedbackFence);
            _feedbackRequested = true;
        }
    }
}

void Msg::Renderer::TexturingSubsystem::_CreateFeedbackBuffers(const glm::uvec2& a_BufferRes)
{
    glm::uvec2 newRes = glm::max(a_BufferRes / 16u, 16u);
    if (newRes == glm::uvec2(_feedbackRes.xy()))
        return;
    _feedbackRes        = glm::uvec3(newRes, SAMPLERS_MATERIAL_COUNT);
    auto feedbackFBInfo = GetFeedbackFBInfo(_feedbackRes);
    feedbackFBInfo.colorBuffers.resize(1);
    feedbackFBInfo.colorBuffers[0].attachment = GL_COLOR_ATTACHMENT0;
    feedbackFBInfo.colorBuffers[0].layer      = 0;
    feedbackFBInfo.colorBuffers[0].texture    = std::make_shared<OGLTexture2DArray>(ctx,
        OGLTexture2DArrayInfo {
            .width       = _feedbackRes.x,
            .height      = _feedbackRes.y,
            .layers      = _feedbackRes.z,
            .sizedFormat = GL_RGB32UI });
    feedbackFBInfo.depthBuffer.texture        = std::make_shared<OGLTexture2DArray>(ctx,
        OGLTexture2DArrayInfo {
            .width       = _feedbackRes.x,
            .height      = _feedbackRes.y,
            .layers      = _feedbackRes.z,
            .sizedFormat = GL_DEPTH_COMPONENT16,
        });
    _feedbackFB                               = std::make_shared<OGLFrameBuffer>(ctx, feedbackFBInfo);
    _feedbackTexBuffer.resize(_feedbackRes.x * _feedbackRes.y * _feedbackRes.z);
    GLSL::VTFeedbackSettings settings = feedbackSettingsBuffer->Get();
    settings.bufferRatio              = glm::vec2(_feedbackRes.x, _feedbackRes.y) / glm::vec2(a_BufferRes);
    feedbackSettingsBuffer->Set(settings);
    feedbackSettingsBuffer->Update();
}

std::shared_ptr<Msg::OGLVertexArray> Msg::Renderer::TexturingSubsystem::_LoadPrimitive(Renderer::Primitive& a_rPrimitive)
{
    auto vaoItr = _VAOs.find(a_rPrimitive.vertexArray);
    if (vaoItr == _VAOs.end()) {
        std::shared_ptr<OGLVertexArray> vao;
        if (a_rPrimitive.vertexArray->indexed)
            vao = std::make_shared<Msg::OGLVertexArray>(ctx,
                a_rPrimitive.vertexArray->vertexCount,
                a_rPrimitive.vertexArray->attributesDesc,
                a_rPrimitive.vertexArray->vertexBindings,
                a_rPrimitive.vertexArray->indexCount,
                a_rPrimitive.vertexArray->indexDesc,
                a_rPrimitive.vertexArray->indexBuffer);
        else
            vao = std::make_shared<Msg::OGLVertexArray>(ctx,
                a_rPrimitive.vertexArray->vertexCount,
                a_rPrimitive.vertexArray->attributesDesc,
                a_rPrimitive.vertexArray->vertexBindings);
        vaoItr = _VAOs.insert({ a_rPrimitive.vertexArray, vao }).first;
    }
    return vaoItr->second;
}
