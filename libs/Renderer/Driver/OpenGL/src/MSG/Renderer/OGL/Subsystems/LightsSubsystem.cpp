#include <MSG/Camera.hpp>
#include <MSG/ECS/Registry.hpp>
#include <MSG/Entity/Camera.hpp>
#include <MSG/Light/PunctualLight.hpp>
#include <MSG/Scene.hpp>
#include <MSG/Transform.hpp>

#include <MSG/Renderer/OGL/Material.hpp>
#include <MSG/Renderer/OGL/Primitive.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/ShaderCompiler.hpp>
#include <MSG/Renderer/OGL/SparseTexture.hpp>

#include <MSG/Renderer/OGL/Components/LightData.hpp>
#include <MSG/Renderer/OGL/Components/Mesh.hpp>
#include <MSG/Renderer/OGL/Components/MeshSkin.hpp>
#include <MSG/Renderer/OGL/Components/Transform.hpp>

#include <MSG/Renderer/OGL/Subsystems/CameraSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/FrameSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/LightsSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/SubsystemLibrary.hpp>

#include <MSG/OGLBuffer.hpp>
#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLPipeline.hpp>
#include <MSG/OGLSampler.hpp>
#include <MSG/OGLTexture.hpp>
#include <MSG/OGLTextureCube.hpp>
#include <MSG/OGLVertexArray.hpp>

#include <FrameInfo.glsl>
#include <LightsIBLInputs.glsl>
#include <LightsShadowInputs.glsl>
#include <LightsVTFS.glsl>

#include <GL/glew.h>

static inline auto GetGraphicsPipeline(
    const MSG::OGLBindings& a_GlobalBindings,
    const MSG::Renderer::Primitive& a_rPrimitive,
    const MSG::Renderer::Material& a_rMaterial,
    const MSG::Renderer::Component::Transform& a_rTransform,
    const MSG::Renderer::Component::MeshSkin* a_rMeshSkin)
{
    MSG::OGLGraphicsPipelineInfo info;
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
        info.bindings.textures[SAMPLERS_MATERIAL + i] = {
            textureSampler.texture,
            textureSampler.sampler,
        };
    }
    return info;
}

static inline auto GetDrawCmd(const MSG::Renderer::Primitive& a_rPrimitive)
{
    MSG::OGLCmdDrawInfo drawCmd;
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

/*
 * @brief VTFS clusters bounding box are generated only once and never change so they're only generated on the CPU
 */
INLINE std::vector<MSG::Renderer::GLSL::VTFSCluster> GenerateVTFSClusters()
{
    constexpr glm::vec3 clusterSize = {
        1.f / VTFS_CLUSTER_X,
        1.f / VTFS_CLUSTER_Y,
        1.f / VTFS_CLUSTER_Z,
    };
    std::vector<MSG::Renderer::GLSL::VTFSCluster> clusters(VTFS_CLUSTER_COUNT);
    for (size_t z = 0; z < VTFS_CLUSTER_Z; ++z) {
        for (size_t y = 0; y < VTFS_CLUSTER_Y; ++y) {
            for (size_t x = 0; x < VTFS_CLUSTER_X; ++x) {
                glm::vec3 NDCMin           = (glm::vec3(x, y, z) * clusterSize) * 2.f - 1.f;
                glm::vec3 NDCMax           = NDCMin + clusterSize * 2.f;
                auto lightClusterIndex     = MSG::Renderer::GLSL::VTFSClusterIndexTo1D({ x, y, z });
                auto& lightCluster         = clusters[lightClusterIndex];
                lightCluster.aabb.minPoint = MSG::Renderer::GLSL::VTFSClusterPosition(NDCMin);
                lightCluster.aabb.maxPoint = MSG::Renderer::GLSL::VTFSClusterPosition(NDCMax);
            }
        }
    }
    return clusters;
}

MSG::Renderer::LightsVTFSBuffer::LightsVTFSBuffer(MSG::OGLContext& a_Ctx)
    : lightsBuffer(std::make_shared<OGLTypedBuffer<GLSL::VTFSLightsBuffer>>(a_Ctx))
    , cluster(std::make_shared<OGLTypedBufferArray<GLSL::VTFSCluster>>(a_Ctx, VTFS_CLUSTER_COUNT, GenerateVTFSClusters().data()))
    , cmdBuffer(a_Ctx)
{
}

MSG::Renderer::LightsVTFS::LightsVTFS(Renderer::Impl& a_Renderer)
    : _cullingProgram(a_Renderer.shaderCompiler.CompileProgram("VTFSCulling"))
    , _buffers({ { a_Renderer.context }, { a_Renderer.context } })
    , buffer(&_buffers.front())
{
}

void MSG::Renderer::LightsVTFS::Prepare()
{
    buffer = &_buffers.at(_currentBuffer);
}

void MSG::Renderer::LightsVTFS::Update(const SubsystemLibrary& a_Subsystems)
{
    auto& cameraSubsystem = a_Subsystems.Get<CameraSubsystem>();
    OGLComputePipelineInfo cp;
    cp.bindings.uniformBuffers[UBO_CAMERA] = { .buffer = cameraSubsystem.buffer, .offset = 0, .size = cameraSubsystem.buffer->size };
    cp.bindings.storageBuffers[0]          = { .buffer = buffer->lightsBuffer, .offset = 0, .size = buffer->lightsBuffer->size };
    cp.bindings.storageBuffers[1]          = { .buffer = buffer->cluster, .offset = 0, .size = buffer->cluster->size };
    cp.shaderState.program                 = _cullingProgram;
    buffer->executionFence.Wait();
    buffer->executionFence.Reset();
    buffer->cmdBuffer.Reset();
    buffer->cmdBuffer.Begin();
    buffer->cmdBuffer.PushCmd<OGLCmdPushPipeline>(cp);
    buffer->cmdBuffer.PushCmd<OGLCmdDispatchCompute>(
        OGLCmdDispatchComputeInfo {
            .workgroupX = VTFS_CLUSTER_COUNT / VTFS_LOCAL_SIZE,
            .workgroupY = 1,
            .workgroupZ = 1,
        });
    buffer->cmdBuffer.PushCmd<OGLCmdMemoryBarrier>(GL_SHADER_STORAGE_BARRIER_BIT, true);
    buffer->cmdBuffer.End();
    buffer->cmdBuffer.Execute(&buffer->executionFence);
    _currentBuffer = (++_currentBuffer) % _buffers.size();
}

MSG::Renderer::LightsIBL::LightsIBL(OGLContext& a_Ctx)
    : buffer(std::make_shared<OGLTypedBuffer<GLSL::LightsIBLUBO>>(a_Ctx))
{
}

MSG::Renderer::LightsShadows::LightsShadows(OGLContext& a_Ctx)
    : dataBuffer(std::make_shared<OGLTypedBuffer<GLSL::ShadowsBase>>(a_Ctx))
    , viewportsBuffer(std::make_shared<OGLTypedBufferArray<GLSL::Camera>>(a_Ctx, 32))
{
}

template <typename LightType>
inline void MSG::Renderer::LightsSubsystem::_PushLight(
    const LightType& a_LightData,
    GLSL::VTFSLightsBuffer& a_VTFS,
    GLSL::LightsIBLUBO&,
    GLSL::ShadowsBase&,
    GLSL::Camera (&)[SHADOW_MAX_VIEWPORTS],
    size_t&,
    const size_t&)
{
    if (a_VTFS.count >= VTFS_BUFFER_MAX) [[unlikely]]
        return;
    a_VTFS.lights[a_VTFS.count] = *reinterpret_cast<const GLSL::LightBase*>(&a_LightData);
    a_VTFS.count++;
}

template <>
inline void MSG::Renderer::LightsSubsystem::_PushLight(
    const Component::LightIBLData& a_LightData,
    GLSL::VTFSLightsBuffer&,
    GLSL::LightsIBLUBO& a_IBL,
    GLSL::ShadowsBase&,
    GLSL::Camera (&)[SHADOW_MAX_VIEWPORTS],
    size_t&,
    const size_t&)
{
    if (a_IBL.count < SAMPLERS_IBL_COUNT) [[likely]] {
        auto& index    = a_IBL.count;
        auto& ibl      = a_IBL.lights[index];
        ibl.commonData = a_LightData.commonData;
        ibl.halfSize   = a_LightData.halfSize;
        for (uint8_t i = 0; i < a_LightData.irradianceCoefficients.size(); i++)
            ibl.irradianceCoefficients[i] = GLSL::vec4(a_LightData.irradianceCoefficients[i], 1.f);
        ibls.textures[index] = a_LightData.specular;
        a_IBL.count++;
    }
}

template <>
inline void MSG::Renderer::LightsSubsystem::_PushLight(
    const Component::LightData& a_LightData,
    GLSL::VTFSLightsBuffer& a_VTFS,
    GLSL::LightsIBLUBO& a_IBL,
    GLSL::ShadowsBase& a_Shadows,
    GLSL::Camera (&a_Viewports)[SHADOW_MAX_VIEWPORTS],
    size_t& a_ViewportIndex,
    const size_t& a_MaxShadows)
{
    if (a_LightData.shadow.has_value() && a_Shadows.count < a_MaxShadows && a_Shadows.count < SAMPLERS_SHADOW_COUNT) [[unlikely]] {
        auto& index     = a_Shadows.count;
        auto& shadow    = a_Shadows.shadows[index];
        auto& glslLight = *std::visit(
            [](auto& a_Data) {
                return reinterpret_cast<const GLSL::LightBase*>(&a_Data);
            },
            a_LightData);
        const auto& depthRange = a_LightData.shadow->depthRanges[a_LightData.shadow->depthRangeIndex_Prev];
        shadow.light           = glslLight;
        shadow.blurRadius      = a_LightData.shadow->blurRadius;
        shadow.bias            = a_LightData.shadow->bias;
        shadow.normalBias      = a_LightData.shadow->normalBias;
        shadow.minDepth        = depthRange->Get(0);
        shadow.maxDepth        = depthRange->Get(1);
        shadow.viewportIndex   = a_ViewportIndex;
        shadow.viewportCount   = a_LightData.shadow->projections.size();
        for (auto& proj : a_LightData.shadow->projections) {
            a_Viewports[a_ViewportIndex] = proj;
            a_ViewportIndex++;
        }
        shadows.texturesDepth[index] = a_LightData.shadow->textureDepth;
        a_Shadows.count++;
        return;
    }
    return std::visit([this, &a_VTFS, &a_IBL, &a_Shadows, &a_Viewports, &a_ViewportIndex, &a_MaxShadows](auto& a_Data) mutable { _PushLight(a_Data, a_VTFS, a_IBL, a_Shadows, a_Viewports, a_ViewportIndex, a_MaxShadows); }, a_LightData);
}

MSG::Renderer::LightsSubsystem::LightsSubsystem(Renderer::Impl& a_Renderer)
    : SubsystemInterface({ typeid(CameraSubsystem), typeid(FrameSubsystem) })
    , vtfs(a_Renderer)
    , ibls(a_Renderer.context)
    , shadows(a_Renderer.context)
    , iblSpecSampler(std::make_shared<OGLSampler>(a_Renderer.context, OGLSamplerParameters { .minFilter = GL_LINEAR_MIPMAP_LINEAR }))
    , shadowSampler(std::make_shared<OGLSampler>(a_Renderer.context, OGLSamplerParameters { .minFilter = GL_LINEAR, .wrapS = GL_CLAMP_TO_BORDER, .wrapT = GL_CLAMP_TO_BORDER, .wrapR = GL_CLAMP_TO_BORDER, .compareMode = GL_COMPARE_REF_TO_TEXTURE, .compareFunc = GL_LEQUAL }))
    , shadowSamplerCube(std::make_shared<OGLSampler>(a_Renderer.context, OGLSamplerParameters { .minFilter = GL_LINEAR, .wrapS = GL_CLAMP_TO_EDGE, .wrapT = GL_CLAMP_TO_EDGE, .wrapR = GL_CLAMP_TO_EDGE, .compareMode = GL_COMPARE_REF_TO_TEXTURE, .compareFunc = GL_LEQUAL }))
    , _cmdBuffer(a_Renderer.context)
{
}

void MSG::Renderer::LightsSubsystem::Update(Renderer::Impl& a_Renderer, const SubsystemLibrary& a_Subsystems)
{
    auto& activeScene          = a_Renderer.activeScene;
    auto& registry             = *activeScene->GetRegistry();
    const auto& visibleLights  = activeScene->GetVisibleEntities().lights;
    const auto& visibleShadows = activeScene->GetVisibleEntities().shadows;

    std::scoped_lock lock(registry.GetLock());
    for (auto& shadow : visibleShadows) {
        auto& lightData = registry.GetComponent<Component::LightData>(shadow);
        lightData.shadow->Update(a_Renderer, registry, shadow, shadow.viewports);
    }
    for (auto& entityID : visibleLights) {
        auto& lightData = registry.GetComponent<Component::LightData>(entityID);
        lightData.Update(a_Renderer, registry, entityID);
    }
    vtfs.Prepare();

    // Update OGL buffers
    GLSL::VTFSLightsBuffer vtfsBuffer = vtfs.buffer->lightsBuffer->Get();
    GLSL::LightsIBLUBO iblBuffer      = ibls.buffer->Get();
    GLSL::ShadowsBase shadowsData     = shadows.dataBuffer->Get();
    GLSL::Camera shadowViewports[SHADOW_MAX_VIEWPORTS];
    size_t shadowViewportsIndex = 0;
    vtfsBuffer.count            = 0;
    iblBuffer.count             = 0;
    shadowsData.count           = 0;
    for (auto& entity : activeScene->GetVisibleEntities().lights)
        _PushLight(registry.GetComponent<Component::LightData>(entity), vtfsBuffer, iblBuffer, shadowsData, shadowViewports, shadowViewportsIndex, visibleShadows.size());
    vtfs.buffer->lightsBuffer->Set(vtfsBuffer);
    ibls.buffer->Set(iblBuffer);
    shadows.dataBuffer->Set(shadowsData);
    shadows.viewportsBuffer->Set(0, shadowViewportsIndex, shadowViewports);

    vtfs.buffer->lightsBuffer->Update();
    ibls.buffer->Update();
    shadows.dataBuffer->Update();
    shadows.viewportsBuffer->Update();

    vtfs.Update(a_Subsystems);
    _UpdateShadows(a_Renderer, a_Subsystems);
}

void MSG::Renderer::LightsSubsystem::_UpdateShadows(Renderer::Impl& a_Renderer, const SubsystemLibrary& a_Subsystems)
{
    auto& activeScene    = *a_Renderer.activeScene;
    auto& registry       = *activeScene.GetRegistry();
    auto& frameSubsystem = a_Subsystems.Get<FrameSubsystem>();
    _executionFence.Wait();
    _executionFence.Reset();
    _cmdBuffer.Reset();
    _cmdBuffer.Begin();
    for (uint32_t i = 0; i < shadows.dataBuffer->Get().count; i++) {
        auto& glslData      = shadows.dataBuffer->Get().shadows[i];
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
            _cmdBuffer.PushCmd<OGLCmdPushRenderPass>(info);
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
                auto& rMesh      = registry.GetComponent<Component::Mesh>(entity).at(entity.lod);
                auto& rTransform = registry.GetComponent<Component::Transform>(entity);
                auto rMeshSkin   = registry.HasComponent<Component::MeshSkin>(entity) ? &registry.GetComponent<Component::MeshSkin>(entity) : nullptr;
                for (auto& [rPrimitive, rMaterial] : rMesh) {
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
                    auto gpInfo                = GetGraphicsPipeline(globalBindings, *rPrimitive, *rMaterial, rTransform, rMeshSkin);
                    gpInfo.shaderState.program = shader;
                    _cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
                    _cmdBuffer.PushCmd<OGLCmdDraw>(GetDrawCmd(*rPrimitive));
                }
            }
            _cmdBuffer.PushCmd<OGLCmdEndRenderPass>();
        }
    }
    _cmdBuffer.End();
    _cmdBuffer.Execute(&_executionFence);
}
