#include <MSG/Renderer/OGL/Material.hpp>
#include <MSG/Renderer/OGL/Primitive.hpp>
#include <MSG/Renderer/OGL/RenderBuffer.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/SparseTexture.hpp>

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

static constexpr Msg::Renderer::GLSL::VTFeedbackOutput s_FeedbackDefaultVal {
    .minUV  = glm::vec2(std::numeric_limits<float>::max()),
    .maxUV  = glm::vec2(std::numeric_limits<float>::lowest()),
    .minMip = std::numeric_limits<float>::max(),
    .maxMip = std::numeric_limits<float>::lowest(),
};

static inline auto GetFeedbackBindings(const Msg::Renderer::SubsystemsLibrary& a_Subsystems, const uint32_t& a_MtlIndex)
{
    auto& cameraSubsystem    = a_Subsystems.Get<Msg::Renderer::CameraSubsystem>();
    auto& texturingSubsystem = a_Subsystems.Get<Msg::Renderer::TexturingSubsystem>();
    auto& mtlBuffer          = texturingSubsystem.feedbackMaterialsBuffer;
    auto& outputBuffer       = texturingSubsystem.feedbackOutputBuffer;
    uint32_t mtlOffset       = sizeof(Msg::Renderer::GLSL::VTMaterialInfo) * a_MtlIndex;
    Msg::OGLBindings bindings;
    bindings.uniformBuffers[UBO_CAMERA] = { cameraSubsystem.buffer, 0, cameraSubsystem.buffer->size };
    bindings.storageBuffers[0]          = { mtlBuffer, mtlOffset, sizeof(Msg::Renderer::GLSL::VTMaterialInfo) };
    bindings.storageBuffers[1]          = { outputBuffer, 0, outputBuffer->size };
    return bindings;
}

static inline auto GetGraphicsPipeline(
    Msg::OGLContext& actx,
    const Msg::OGLBindings& a_GlobalBindings,
    const Msg::Renderer::Primitive& a_rPrimitive,
    const Msg::Renderer::Material& a_rMaterial,
    const Msg::Renderer::Mesh& a_rMesh,
    const Msg::Renderer::MeshSkin* a_rMeshSkin)
{
    Msg::OGLGraphicsPipelineInfo info;
    info.bindings                               = a_GlobalBindings;
    info.bindings.uniformBuffers[UBO_TRANSFORM] = { a_rMesh.transform, 0, a_rMesh.transform->size };
    info.inputAssemblyState.primitiveTopology   = a_rPrimitive.drawMode;
    if (a_rPrimitive.vertexArray->indexed)
        info.vertexInputState.vertexArray = std::make_shared<Msg::OGLVertexArray>(actx,
            a_rPrimitive.vertexArray->vertexCount,
            a_rPrimitive.vertexArray->attributesDesc,
            a_rPrimitive.vertexArray->vertexBindings,
            a_rPrimitive.vertexArray->indexCount,
            a_rPrimitive.vertexArray->indexDesc,
            a_rPrimitive.vertexArray->indexBuffer);
    else
        info.vertexInputState.vertexArray = std::make_shared<Msg::OGLVertexArray>(actx,
            a_rPrimitive.vertexArray->vertexCount,
            a_rPrimitive.vertexArray->attributesDesc,
            a_rPrimitive.vertexArray->vertexBindings);
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
    , _feedbackProgram(a_Renderer.shaderCompiler.CompileProgram("VTFeedback", ShaderLibrary::ProgramKeywords { { "SKINNED", "0" } }))
    , _feedbackProgramSkinned(a_Renderer.shaderCompiler.CompileProgram("VTFeedback", ShaderLibrary::ProgramKeywords { { "SKINNED", "1" } }))
    , _feedbackFence(true)
    , _feedbackCmdBuffer(ctx, OGLCmdBufferType::OneShot)
{
    glm::uvec3 currentRes              = glm::vec3(64, 64, 1);
    auto feedbackFBInfo                = GetFeedbackFBInfo(currentRes);
    feedbackFBInfo.depthBuffer.texture = std::make_shared<OGLTexture2D>(ctx,
        OGLTexture2DInfo {
            .width       = feedbackFBInfo.defaultSize.x,
            .height      = feedbackFBInfo.defaultSize.y,
            .sizedFormat = GL_DEPTH_COMPONENT16,
        });
    _feedbackFB                        = std::make_shared<OGLFrameBuffer>(ctx, feedbackFBInfo);
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
    if (!_needsUpdate.load())
        return; // early bail if the upload function did not run yet
    const auto now         = std::chrono::system_clock::now();
    const auto elapsedTime = now - _lastUpdate;
    if (elapsedTime >= SparseTexturePollingRate) {
        _lastUpdate              = now;
        auto& activeRenderBuffer = (*a_Renderer.activeRenderBuffer);
        auto& activeScene        = a_Renderer.activeScene;
        auto& registry           = *activeScene->GetRegistry();
        // create a new feedback pass
        std::unordered_map<uint32_t, std::shared_ptr<SparseTexture>> feedbackIDToTex;
        std::unordered_map<std::shared_ptr<SparseTexture>, uint32_t> feedbackTexToID;
        auto& feedbackCmdBuffer  = _feedbackCmdBuffer;
        auto& feedbackFence      = _feedbackFence;
        auto& visibleEntities    = activeScene->GetVisibleEntities();
        uint32_t curTexID        = 0;
        feedbackTexToID[nullptr] = 0;
        feedbackIDToTex[0]       = nullptr;
        std::unordered_map<const Material*, uint32_t> materialsID;
        std::vector<GLSL::VTMaterialInfo> materials;
        materialsID.reserve(1024);
        materials.reserve(1024);
        for (auto& entity : visibleEntities.meshes) {
            auto& rMaterials = registry.GetComponent<Renderer::MaterialSet>(entity);
            auto& rMesh      = registry.GetComponent<Renderer::Mesh>(entity);
            for (auto& [rPrimitive, mtlIndex] : rMesh.at(entity.lod)) {
                auto& rMaterial = rMaterials[mtlIndex];
                auto mtlIDItr   = materialsID.find(rMaterial.get());
                if (mtlIDItr != materialsID.end())
                    continue;
                mtlIDItr      = materialsID.insert({ rMaterial.get(), uint32_t(materials.size()) }).first;
                auto& mtlInfo = materials.emplace_back();
                auto& glslMat = rMaterial->buffer->Get();
                for (uint32_t i = 0; i < SAMPLERS_MATERIAL_COUNT; i++) {
                    auto texture = rMaterial->textureSamplers[i].texture;
                    if (texture != nullptr && !texture->sparse)
                        texture = nullptr;
                    auto sampler  = rMaterial->textureSamplers[i].sampler.get();
                    auto maxAniso = sampler != nullptr ? sampler->maxAnisotropy : 0;
                    auto lodBias  = sampler != nullptr ? sampler->lodBias : 0.f;
                    auto wrapS    = sampler != nullptr ? sampler->wrapS : GL_REPEAT;
                    auto wrapT    = sampler != nullptr ? sampler->wrapT : GL_REPEAT;
                    auto itr      = feedbackTexToID.find(texture);
                    if (itr == feedbackTexToID.end()) {
                        curTexID++;
                        itr                       = feedbackTexToID.insert({ texture, curTexID }).first;
                        feedbackIDToTex[curTexID] = texture;
                    }
                    auto textureID      = itr->second;
                    mtlInfo.textures[i] = {
                        .transform = glslMat.textureInfos[i].transform,
                        .texCoord  = glslMat.textureInfos[i].texCoord,
                        .id        = textureID,
                        .wrapS     = GetVTWrapMode(wrapS),
                        .wrapT     = GetVTWrapMode(wrapT),
                        .maxAniso  = maxAniso,
                        .lodBias   = lodBias,
                        .texSize   = texture != nullptr ? glm::vec2(texture->src->GetSize()) : glm::vec2(0)
                    };
                }
            }
        }
        if (!materials.empty()) {
            if (feedbackOutputBuffer == nullptr || feedbackOutputBuffer->GetCount() < feedbackIDToTex.size())
                feedbackOutputBuffer = std::make_shared<OGLTypedBufferArray<GLSL::VTFeedbackOutput>>(ctx, feedbackIDToTex.size());
            // reset feedback buffer
            for (size_t i = 0; i < feedbackOutputBuffer->GetCount(); i++)
                feedbackOutputBuffer->Set(i, s_FeedbackDefaultVal);
            feedbackOutputBuffer->Update();
            if (feedbackMaterialsBuffer == nullptr || feedbackMaterialsBuffer->GetCount() < materials.size())
                feedbackMaterialsBuffer = std::make_shared<OGLTypedBufferArray<GLSL::VTMaterialInfo>>(ctx, materials.size());
            feedbackMaterialsBuffer->Set(0, materials.size(), materials.data());
            feedbackMaterialsBuffer->Update();
            OGLRenderPassInfo renderPass;
            renderPass.frameBufferState.clear.depth = 1.f;
            renderPass.frameBufferState.framebuffer = _feedbackFB;
            renderPass.viewportState.viewportExtent = _feedbackFB->info.defaultSize;
            renderPass.viewportState.scissorExtent  = _feedbackFB->info.defaultSize;
            feedbackCmdBuffer.Begin();
            feedbackCmdBuffer.PushCmd<OGLCmdPushRenderPass>(renderPass);
            for (auto& entity : visibleEntities.meshes) {
                auto& sgMesh    = registry.GetComponent<Msg::Mesh>(entity);
                auto& sgMeshLod = sgMesh.at(entity.lod);
                auto& rMesh     = registry.GetComponent<Renderer::Mesh>(entity);
                auto rMaterials = registry.GetComponent<Renderer::MaterialSet>(entity);
                auto rMeshSkin  = registry.HasComponent<Renderer::MeshSkin>(entity) ? &registry.GetComponent<Renderer::MeshSkin>(entity) : nullptr;
                for (auto& [rPrimitive, mtlIndex] : rMesh.at(entity.lod)) {
                    auto& rMaterial = rMaterials[mtlIndex];
                    auto mtlID      = materialsID.at(rMaterial.get());
                    auto gp         = GetGraphicsPipeline(
                        ctx,
                        GetFeedbackBindings(a_Subsystems, mtlID),
                        *rPrimitive, *rMaterial,
                        rMesh, rMeshSkin);
                    gp.shaderState.program = rMeshSkin != nullptr ? _feedbackProgramSkinned : _feedbackProgram;
                    feedbackCmdBuffer.PushCmd<OGLCmdPushPipeline>(gp);
                    feedbackCmdBuffer.PushCmd<OGLCmdDraw>(GetDrawCmd(*rPrimitive));
                }
            }
            feedbackCmdBuffer.PushCmd<OGLCmdEndRenderPass>();
            feedbackCmdBuffer.PushCmd<OGLCmdMemoryBarrier>(GL_SHADER_STORAGE_BARRIER_BIT, true);
            feedbackCmdBuffer.End();
            feedbackFence.Reset();
            feedbackCmdBuffer.Execute(&feedbackFence);
            feedbackFence.Wait();
            feedbackOutputBuffer->Read();
            // upload necessary textures here
            for (size_t texID = 1; texID < feedbackIDToTex.size(); texID++) {
                auto& tex         = feedbackIDToTex.at(texID);
                auto& feedbackRes = feedbackOutputBuffer->Get(texID);
                tex->RequestPages(
                    feedbackRes.minMip, feedbackRes.maxMip,
                    glm::vec3(feedbackRes.minUV, 0), glm::vec3(feedbackRes.maxUV, 1));
                _managedTextures.insert(tex);
            }
        }
    }
    if (!_managedTextures.empty()) {
        _needsUpdate.store(false);
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
            _needsUpdate.store(true);
        });
    }
}
