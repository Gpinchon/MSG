#pragma once

#include <Renderer/Handles.hpp>

#include <Renderer/OGL/UniformBuffer.hpp>

#include <Bindings.glsl>
#include <Material.glsl>

#include <memory>
#include <array>

namespace TabGraph::SG {
class Material;
struct BaseExtension;
struct SpecularGlossinessExtension;
struct MetallicRoughnessExtension;
}

namespace TabGraph::Renderer::RAII {
class Texture;
class Sampler;
}

namespace TabGraph::Renderer {
struct MaterialUBO {
    union {
        GLSL::BaseMaterial base = {};
        GLSL::MetallicRoughnessMaterial metallicRoughness;
        GLSL::SpecularGlossinessMaterial specularGlossiness;
    };
    GLSL::TextureInfo textureInfos[SAMPLERS_MATERIAL_COUNT];
};

class Material : public UniformBufferT<MaterialUBO> {
public:
    struct TextureSampler {
        std::shared_ptr<RAII::Texture> texture;
        std::shared_ptr<RAII::Sampler> sampler;
    };
    Material(Context& a_Context)
        : UniformBufferT(a_Context) {};
    void Set(Renderer::Impl& a_Renderer, const SG::Material& a_SGMaterial);
    int type = MATERIAL_TYPE_UNKNOWN;
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
