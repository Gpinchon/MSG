#pragma once

#include <Renderer/Handles.hpp>

#include <Renderer/OGL/UniformBuffer.hpp>

#include <Bindings.glsl>
#include <Material.glsl>

#include <memory>
#include <variant>

namespace TabGraph::SG {
class Material;
struct BaseExtension;
struct SpecularGlossinessExtension;
}

namespace TabGraph::Renderer::RAII {
struct Texture;
struct Sampler;
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

struct Material : UniformBufferT<MaterialUBO> {
    struct TextureSampler {
        std::shared_ptr<RAII::Texture> texture;
        std::shared_ptr<RAII::Sampler> sampler;
    };
    enum class Type {
        Unknown = -1,
        Base,
        MetallicRoughness,
        SpecularGlossiness,
        MaxValue
    };
    Material(Context& a_Context)
        : UniformBufferT(a_Context) {};
    void Set(Renderer::Impl& a_Renderer, const SG::Material& a_SGMaterial);
    Type type = Type::Unknown;
    std::array<TextureSampler, SAMPLERS_MATERIAL_COUNT> textureSamplers;

private:
    void _LoadBaseExtension(
        Renderer::Impl& a_Renderer,
        const SG::BaseExtension& a_Extension);
    void _LoadSpecGlossExtension(
        Renderer::Impl& a_Renderer,
        const SG::SpecularGlossinessExtension& a_Extension);
};

}
