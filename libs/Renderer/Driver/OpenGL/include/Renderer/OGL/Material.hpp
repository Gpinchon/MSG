#pragma once

#include <Renderer/Handles.hpp>
#include <Renderer/OGL/UniformBuffer.hpp>

#include <Bindings.glsl>
#include <Material.glsl>

#include <array>
#include <memory>

namespace MSG::SG {
class Material;
struct BaseExtension;
struct SpecularGlossinessExtension;
struct MetallicRoughnessExtension;
}

namespace MSG::Renderer::RAII {
class Texture;
class Sampler;
}

namespace MSG::Renderer {
struct MaterialUBO {
    union {
        GLSL::BaseMaterial base = {};
        GLSL::MetallicRoughnessMaterial metallicRoughness;
        GLSL::SpecularGlossinessMaterial specularGlossiness;
    };
    GLSL::TextureInfo textureInfos[SAMPLERS_MATERIAL_COUNT];
};

struct TextureSampler {
    std::shared_ptr<RAII::Texture> texture;
    std::shared_ptr<RAII::Sampler> sampler;
};

class Material : public UniformBufferT<MaterialUBO> {
public:
    Material(Context& a_Context)
        : UniformBufferT(a_Context) {};
    void Set(Renderer::Impl& a_Renderer, const SG::Material& a_SGMaterial);
    int type         = MATERIAL_TYPE_UNKNOWN;
    int alphaMode    = -1;
    bool doubleSided = false;
    bool unlit       = false;
    std::array<TextureSampler, SAMPLERS_MATERIAL_COUNT> textureSamplers;

private:
    void _LoadBaseExtension(
        Renderer::Impl& a_Renderer,
        const SG::BaseExtension& a_Extension);
    void _LoadSpecGlossExtension(
        Renderer::Impl& a_Renderer,
        const SG::SpecularGlossinessExtension& a_Extension);
    void _LoadMetRoughExtension(
        Renderer::Impl& a_Renderer,
        const SG::MetallicRoughnessExtension& a_Extension);
};

}
