#include <MSG/Renderer/OGL/Subsystems/MeshSubsystem.hpp>

#include <MSG/Renderer/OGL/Material.hpp>
#include <MSG/Renderer/OGL/Primitive.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/SparseTexture.hpp>

#include <MSG/Renderer/OGL/Components/Mesh.hpp>
#include <MSG/Renderer/OGL/Components/MeshSkin.hpp>

#include <MSG/Renderer/OGL/Subsystems/CameraSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/FogSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/FrameSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/LightsSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/MaterialSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/SkinSubsystem.hpp>
#include <MSG/Renderer/SubsystemInterface.hpp>

#include <MSG/BRDF.hpp>
#include <MSG/ECS/Registry.hpp>
#include <MSG/MaterialSet.hpp>
#include <MSG/Mesh.hpp>
#include <MSG/Scene.hpp>
#include <MSG/Texture.hpp>

#include <MSG/Tools/LazyConstructor.hpp>

#include <MSG/OGLSampler.hpp>
#include <MSG/OGLTexture3D.hpp>
#include <MSG/OGLTextureCube.hpp>
#include <MSG/OGLVertexArray.hpp>

#include <Bindings.glsl>
#include <Fog.glsl>
#include <FogCamera.glsl>
#include <FrameInfo.glsl>
#include <LightsIBLInputs.glsl>
#include <LightsShadowInputs.glsl>
#include <LightsVTFS.glsl>

#include <glm/gtc/matrix_inverse.hpp>

static inline auto GetGlobalBindings(const Msg::Renderer::SubsystemsLibrary& a_Subsystems)
{
    auto& lightSubsystem  = a_Subsystems.Get<Msg::Renderer::LightsSubsystem>();
    auto& frameSubsystem  = a_Subsystems.Get<Msg::Renderer::FrameSubsystem>();
    auto& cameraSubsystem = a_Subsystems.Get<Msg::Renderer::CameraSubsystem>();
    auto& fogSubsystem    = a_Subsystems.Get<Msg::Renderer::FogSubsystem>();
    auto& meshSubsystem   = a_Subsystems.Get<Msg::Renderer::MeshSubsystem>();
    Msg::OGLBindings bindings;
    bindings.uniformBuffers[UBO_FRAME_INFO]        = { frameSubsystem.buffer, 0, frameSubsystem.buffer->size };
    bindings.uniformBuffers[UBO_CAMERA]            = { cameraSubsystem.buffer, 0, cameraSubsystem.buffer->size };
    bindings.uniformBuffers[UBO_FOG_CAMERA]        = { fogSubsystem.fogCamerasBuffer, 0, fogSubsystem.fogCamerasBuffer->size };
    bindings.uniformBuffers[UBO_FOG_SETTINGS]      = { fogSubsystem.fogSettingsBuffer, 0, fogSubsystem.fogSettingsBuffer->size };
    bindings.uniformBuffers[UBO_FWD_IBL]           = { lightSubsystem.ibls.buffer, 0, lightSubsystem.ibls.buffer->size };
    bindings.storageBuffers[SSBO_SHADOW_DATA]      = { lightSubsystem.shadows.dataBuffer, 0, lightSubsystem.shadows.dataBuffer->size };
    bindings.storageBuffers[SSBO_SHADOW_VIEWPORTS] = { lightSubsystem.shadows.viewportsBuffer, 0, lightSubsystem.shadows.viewportsBuffer->size };
    bindings.storageBuffers[SSBO_VTFS_LIGHTS]      = { lightSubsystem.vtfs.buffer->lightsBuffer, offsetof(Msg::Renderer::GLSL::VTFSLightsBuffer, lights), lightSubsystem.vtfs.buffer->lightsBuffer->size };
    bindings.storageBuffers[SSBO_VTFS_CLUSTERS]    = { lightSubsystem.vtfs.buffer->cluster, 0, lightSubsystem.vtfs.buffer->cluster->size };
    bindings.textures[SAMPLERS_BRDF_LUT]           = { meshSubsystem.brdfLut, meshSubsystem.brdfLutSampler };
    for (uint32_t i = 0; i < fogSubsystem.textures.size(); i++)
        bindings.textures[SAMPLERS_FOG + i] = { fogSubsystem.textures[i].resultTexture, fogSubsystem.sampler };
    for (auto i = 0u; i < lightSubsystem.ibls.buffer->Get().count; i++)
        bindings.textures[SAMPLERS_IBL + i] = { .texture = lightSubsystem.ibls.textures.at(i), .sampler = lightSubsystem.iblSpecSampler };
    for (auto i = 0u; i < lightSubsystem.shadows.dataBuffer->Get().count; i++) {
        auto& glslLight     = lightSubsystem.shadows.dataBuffer->Get().shadows[i];
        auto& glslLightType = glslLight.light.commonData.type;
        auto& sampler       = glslLightType == LIGHT_TYPE_POINT ? lightSubsystem.shadowSamplerCube : lightSubsystem.shadowSampler;

        bindings.textures[SAMPLERS_SHADOW + i] = { .texture = lightSubsystem.shadows.texturesDepth[i], .sampler = sampler };
    }
    return bindings;
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

Msg::Renderer::MeshSubsystem::MeshSubsystem(Renderer::Impl& a_Renderer)
    : SubsystemInterface({
          typeid(LightsSubsystem),
          typeid(FrameSubsystem),
          typeid(CameraSubsystem),
          typeid(FogSubsystem),
          typeid(MaterialSubsystem),
          typeid(SkinSubsystem),
      })
    , brdfLut(a_Renderer.LoadTexture(new Texture(BRDF::GenerateTexture(BRDF::Type::Standard))))
    , brdfLutSampler(std::make_shared<OGLSampler>(a_Renderer.context, OGLSamplerParameters { .wrapS = GL_CLAMP_TO_EDGE, .wrapT = GL_CLAMP_TO_EDGE, .wrapR = GL_CLAMP_TO_EDGE }))
{
}

Msg::Renderer::MeshSubsystem::~MeshSubsystem()
{
    CleanupCache();
    MSGCheckErrorFatal(!primitiveCache.empty(), "Not all primitives were unloaded !");
}

const std::shared_ptr<Msg::Renderer::Primitive>& Msg::Renderer::MeshSubsystem::LoadPrimitive(Renderer::Impl& a_Renderer, Msg::MeshPrimitive* const a_Primitive)
{
    return primitiveCache.GetOrCreate(a_Primitive,
        Tools::LazyConstructor(
            [&a_Renderer, a_Primitive]() {
                return std::make_shared<Renderer::Primitive>(a_Renderer.context, *a_Primitive);
            }));
}

void Msg::Renderer::MeshSubsystem::Load(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    if (a_Entity.HasComponent<Msg::Mesh>() && !a_Entity.HasComponent<Renderer::Mesh>()) {
        std::vector<Renderer::MeshLod> rMeshLods;
        const auto& sgMesh      = a_Entity.GetComponent<Msg::Mesh>();
        const auto& sgTransform = a_Entity.HasComponent<Msg::Transform>() ? a_Entity.GetComponent<Msg::Transform>() : Msg::Transform {};
        auto& materials         = a_Entity.GetComponent<MaterialSet>();
        for (auto& sgMeshLod : sgMesh) {
            Renderer::MeshLod rMeshLod;
            for (auto& [sgPrimitive, mtlIndex] : sgMeshLod)
                rMeshLod.emplace_back(LoadPrimitive(a_Renderer, sgPrimitive.get()), mtlIndex);
            rMeshLods.emplace_back(rMeshLod);
        }
        GLSL::TransformUBO transform   = {};
        transform.current.modelMatrix  = sgMesh.geometryTransform * sgTransform.GetWorldTransformMatrix();
        transform.current.normalMatrix = glm::inverseTranspose(glm::mat3(transform.current.modelMatrix));
        transform.previous             = transform.current;
        a_Entity.AddComponent<Renderer::Mesh>(a_Renderer.context, rMeshLods, transform);
    }
}

void Msg::Renderer::MeshSubsystem::Unload(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    if (a_Entity.HasComponent<Renderer::Mesh>())
        a_Entity.RemoveComponent<Renderer::Mesh>();
    if (a_Entity.HasComponent<Msg::Mesh>()) {
        auto& sgMesh = a_Entity.GetComponent<Msg::Mesh>();
        for (auto& sgMeshLod : sgMesh) {
            for (auto& [sgPrimitive, mtlIndex] : sgMeshLod) {
                auto itr = primitiveCache.find(sgPrimitive.get());
                MSGCheckErrorFatal(itr == primitiveCache.end(), "Mesh \"" + std::string(sgMesh.name) + "\" primitive not loaded, how did this happen ?!");
                if (itr != primitiveCache.end() && itr->second.use_count() == 1)
                    primitiveCache.erase(itr);
            }
        }
    }
}

void Msg::Renderer::MeshSubsystem::Update(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems)
{
    globalBindings     = GetGlobalBindings(a_Subsystems);
    auto& mtlSubsystem = a_Subsystems.Get<MaterialSubsystem>();
    auto& activeScene  = *a_Renderer.activeScene;
    auto& registry     = *activeScene.GetRegistry();
    opaque.clear();
    blended.clear();
    opaque.reserve(activeScene.GetVisibleEntities().meshes.size());
    blended.reserve(activeScene.GetVisibleEntities().meshes.size());
    for (auto& entity : activeScene.GetVisibleEntities().meshes) {
        auto& sgMesh      = registry.GetComponent<Msg::Mesh>(entity);
        auto& sgTransform = registry.GetComponent<Msg::Transform>(entity);
        if (!registry.HasComponent<Renderer::Mesh>(entity)) {
            MSGErrorWarning("Mesh \"" + std::string(sgMesh.name) + "\" not loaded, loading now");
            Load(a_Renderer, registry.GetEntityRef(entity));
        }
        auto& rMesh      = registry.GetComponent<Renderer::Mesh>(entity);
        auto& rMaterials = registry.GetComponent<Renderer::MaterialSet>(entity);
        auto rMeshSkin   = registry.HasComponent<Renderer::MeshSkin>(entity) ? &registry.GetComponent<Renderer::MeshSkin>(entity) : nullptr;
        for (auto& [rPrimitive, mtlIndex] : rMesh.at(entity.lod)) {
            auto& rMaterial = *rMaterials[mtlIndex];
            MeshInfo* meshInfo;
            if (rMaterial.alphaMode == MATERIAL_ALPHA_BLEND)
                meshInfo = &blended.emplace_back();
            else
                meshInfo = &opaque.emplace_back();
            meshInfo->pipeline    = GetGraphicsPipeline(globalBindings, *rPrimitive, rMaterial, rMesh, rMeshSkin);
            meshInfo->drawCmd     = GetDrawCmd(*rPrimitive);
            meshInfo->isMetRough  = rMaterial.type == MATERIAL_TYPE_METALLIC_ROUGHNESS;
            meshInfo->isSpecGloss = rMaterial.type == MATERIAL_TYPE_SPECULAR_GLOSSINESS;
            meshInfo->isUnlit     = rMaterial.unlit;
        }
        GLSL::TransformUBO transformUBO   = rMesh.transform->Get();
        transformUBO.previous             = transformUBO.current;
        transformUBO.current.modelMatrix  = sgMesh.geometryTransform * sgTransform.GetWorldTransformMatrix();
        transformUBO.current.normalMatrix = glm::inverseTranspose(transformUBO.current.modelMatrix);
        rMesh.transform->Set(transformUBO);
        rMesh.transform->Update();
    }
    CleanupCache();
}

void Msg::Renderer::MeshSubsystem::CleanupCache()
{
    auto itr = primitiveCache.begin();
    while (itr != primitiveCache.end()) {
        if (itr->second.use_count() == 1) {
            itr = primitiveCache.erase(itr);
        } else
            itr++;
    }
}