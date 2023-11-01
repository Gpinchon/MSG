#pragma once

#include <Renderer/Handles.hpp>

#include <Renderer/OGL/GLSL/MaterialUBO.hpp>
#include <Renderer/OGL/UniformBuffer.hpp>

#include <memory>
#include <variant>

namespace TabGraph::SG {
class Material;
struct BaseExtension;
struct SpecularGlossinessExtension;
}

namespace TabGraph::Renderer::RAII {
struct TextureSampler;
}

namespace TabGraph::Renderer {
struct TextureSamplerLoader;
}

namespace TabGraph::Renderer {
union MaterialUBO {
    GLSL::BaseMaterial base = {};
    GLSL::MetallicRoughnessMaterial metallicRoughness;
    GLSL::SpecularGlossinessMaterial specularGlossiness;
};

struct Material : UniformBufferT<MaterialUBO> {
    enum class Type {
        Unknown = -1,
        Base,
        MetallicRoughness,
        SpecularGlossiness,
        MaxValue
    };
    Material(RAII::Context& a_Context)
        : UniformBufferT(a_Context) {};
    void Set(Renderer::Impl& a_Renderer, const SG::Material& a_SGMaterial);
    Type type = Type::Unknown;

private:
    void _LoadBaseExtension(
        Renderer::Impl& a_Renderer,
        const SG::BaseExtension& a_Extension);
    void _LoadSpecGlossExtension(
        Renderer::Impl& a_Renderer,
        const SG::SpecularGlossinessExtension& a_Extension);
    struct BaseTextures {
        std::shared_ptr<RAII::TextureSampler> normal;
        std::shared_ptr<RAII::TextureSampler> occlusion;
        std::shared_ptr<RAII::TextureSampler> emissive;
    };
    struct MetallicRoughnessTextures : BaseTextures {
        std::shared_ptr<RAII::TextureSampler> albedo;
        std::shared_ptr<RAII::TextureSampler> metallicRoughness;
    };
    struct SpecularGlossinessTextures : BaseTextures {
        std::shared_ptr<RAII::TextureSampler> diffuse;
        std::shared_ptr<RAII::TextureSampler> specularGlossiness;
    };
    std::variant<BaseTextures, MetallicRoughnessTextures, SpecularGlossinessTextures> _textures;
};

}
