#pragma once

#include <MSG/OGLTypedBuffer.hpp>
#include <MSG/Renderer/Handles.hpp>

#include <Bindings.glsl>
#include <Material.glsl>
#include <VirtualTexturing.glsl>

#include <array>
#include <memory>

namespace Msg::Renderer {
class VirtualTexture;
}

namespace Msg {
class Material;
class Texture;
class Sampler;
struct MaterialTextureInfo;
struct MaterialExtensionBase;
struct MaterialExtensionSpecularGlossiness;
struct MaterialExtensionMetallicRoughness;
class OGLSampler;
class OGLTexture;
}

namespace Msg::Renderer {
struct MaterialUBO {
    union {
        GLSL::BaseMaterial base = {};
        GLSL::MetallicRoughnessMaterial metallicRoughness;
        GLSL::SpecularGlossinessMaterial specularGlossiness;
    };
    GLSL::VTInfo textureInfos[SAMPLERS_MATERIAL_COUNT];
};

class Material {
public:
    Material(OGLContext& a_Context)
        : buffer(std::make_shared<OGLTypedBuffer<MaterialUBO>>(a_Context)) { };
    static std::shared_ptr<OGLSampler> GetPageTableSampler(Renderer::Impl& a_Renderer);
    void Set(Renderer::Impl& a_Renderer, const Msg::Material& a_SGMaterial);
    int type             = MATERIAL_TYPE_UNKNOWN;
    bool doubleSided     = false;
    bool unlit           = false;
    bool virtualTextures = true;
    std::shared_ptr<OGLTypedBuffer<MaterialUBO>> buffer;
    std::array<std::shared_ptr<VirtualTexture>, SAMPLERS_MATERIAL_COUNT> textures;
    std::array<std::shared_ptr<OGLTexture>, SAMPLERS_MATERIAL_COUNT> pageTables;
    std::shared_ptr<OGLTexture> textureAtlas;

private:
    void _FillTextureData(
        Renderer::Impl& a_Renderer,
        MaterialUBO& a_UBO, const uint32_t& a_SamplerIndex,
        const MaterialTextureInfo& a_SGTexInfo, const std::shared_ptr<Texture>& a_SGTexture, const std::shared_ptr<Sampler>& a_SGSampler);
    void _LoadBaseExtension(
        Renderer::Impl& a_Renderer,
        const MaterialExtensionBase& a_Extension);
    void _LoadSpecGlossExtension(
        Renderer::Impl& a_Renderer,
        const MaterialExtensionSpecularGlossiness& a_Extension);
    void _LoadMetRoughExtension(
        Renderer::Impl& a_Renderer,
        const MaterialExtensionMetallicRoughness& a_Extension);
};
}
