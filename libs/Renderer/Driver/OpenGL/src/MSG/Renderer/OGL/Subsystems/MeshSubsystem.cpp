#include <MSG/Renderer/OGL/Subsystems/MeshSubsystem.hpp>

#include <MSG/Renderer/OGL/Material.hpp>
#include <MSG/Renderer/OGL/Primitive.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/SparseTexture.hpp>

#include <MSG/Renderer/OGL/Components/Mesh.hpp>
#include <MSG/Renderer/OGL/Components/MeshSkin.hpp>
#include <MSG/Renderer/OGL/Components/Transform.hpp>

#include <MSG/Renderer/OGL/Subsystems/CameraSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/FogSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/FrameSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/LightsSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/MaterialSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/SkinSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/SubsystemLibrary.hpp>

#include <MSG/BRDF.hpp>
#include <MSG/ECS/Registry.hpp>
#include <MSG/Scene.hpp>
#include <MSG/Texture.hpp>

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

static inline auto GetGlobalBindings(const MSG::Renderer::SubsystemLibrary& a_Subsystems)
{
    auto& lightSubsystem  = a_Subsystems.Get<MSG::Renderer::LightsSubsystem>();
    auto& frameSubsystem  = a_Subsystems.Get<MSG::Renderer::FrameSubsystem>();
    auto& cameraSubsystem = a_Subsystems.Get<MSG::Renderer::CameraSubsystem>();
    auto& fogSubsystem    = a_Subsystems.Get<MSG::Renderer::FogSubsystem>();
    auto& meshSubsystem   = a_Subsystems.Get<MSG::Renderer::MeshSubsystem>();
    MSG::OGLBindings bindings;
    bindings.uniformBuffers[UBO_FRAME_INFO]        = { frameSubsystem.buffer, 0, frameSubsystem.buffer->size };
    bindings.uniformBuffers[UBO_CAMERA]            = { cameraSubsystem.buffer, 0, cameraSubsystem.buffer->size };
    bindings.uniformBuffers[UBO_FOG_CAMERA]        = { fogSubsystem.fogCamerasBuffer, 0, fogSubsystem.fogCamerasBuffer->size };
    bindings.uniformBuffers[UBO_FOG_SETTINGS]      = { fogSubsystem.fogSettingsBuffer, 0, fogSubsystem.fogSettingsBuffer->size };
    bindings.uniformBuffers[UBO_FWD_IBL]           = { lightSubsystem.ibls.buffer, 0, lightSubsystem.ibls.buffer->size };
    bindings.storageBuffers[SSBO_SHADOW_DATA]      = { lightSubsystem.shadows.dataBuffer, 0, lightSubsystem.shadows.dataBuffer->size };
    bindings.storageBuffers[SSBO_SHADOW_VIEWPORTS] = { lightSubsystem.shadows.viewportsBuffer, 0, lightSubsystem.shadows.viewportsBuffer->size };
    bindings.storageBuffers[SSBO_VTFS_LIGHTS]      = { lightSubsystem.vtfs.buffer->lightsBuffer, offsetof(MSG::Renderer::GLSL::VTFSLightsBuffer, lights), lightSubsystem.vtfs.buffer->lightsBuffer->size };
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
            textureSampler.texture != nullptr ? textureSampler.texture->texture : nullptr,
            textureSampler.sampler,
        };
    }
    return info;
}

MSG::Renderer::MeshSubsystem::MeshSubsystem(Renderer::Impl& a_Renderer)
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

void MSG::Renderer::MeshSubsystem::Update(Renderer::Impl& a_Renderer, const SubsystemLibrary& a_Subsystems)
{
    globalBindings    = GetGlobalBindings(a_Subsystems);
    auto& activeScene = *a_Renderer.activeScene;
    auto& registry    = *activeScene.GetRegistry();
    opaque.clear();
    blended.clear();
    opaque.reserve(activeScene.GetVisibleEntities().meshes.size());
    blended.reserve(activeScene.GetVisibleEntities().meshes.size());
    for (auto& entity : activeScene.GetVisibleEntities().meshes) {
        auto& rMesh      = registry.GetComponent<Component::Mesh>(entity).at(entity.lod);
        auto& rTransform = registry.GetComponent<Component::Transform>(entity);
        auto rMeshSkin   = registry.HasComponent<Component::MeshSkin>(entity) ? &registry.GetComponent<Component::MeshSkin>(entity) : nullptr;
        for (auto& [rPrimitive, rMaterial] : rMesh) {
            const bool isAlphaBlend = rMaterial->alphaMode == MATERIAL_ALPHA_BLEND;
            MeshInfo* meshInfo;
            if (isAlphaBlend)
                meshInfo = &blended.emplace_back();
            else
                meshInfo = &opaque.emplace_back();
            meshInfo->pipeline    = GetGraphicsPipeline(globalBindings, *rPrimitive, *rMaterial, rTransform, rMeshSkin);
            meshInfo->drawCmd     = GetDrawCmd(*rPrimitive);
            meshInfo->isMetRough  = rMaterial->type == MATERIAL_TYPE_METALLIC_ROUGHNESS;
            meshInfo->isSpecGloss = rMaterial->type == MATERIAL_TYPE_SPECULAR_GLOSSINESS;
            meshInfo->isUnlit     = rMaterial->unlit;
        }
    }
}