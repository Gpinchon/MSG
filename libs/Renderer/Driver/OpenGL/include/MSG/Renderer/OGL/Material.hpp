#pragma once

#include <MSG/Renderer/Handles.hpp>
#include <MSG/Renderer/OGL/UniformBuffer.hpp>

#include <Bindings.glsl>
#include <Material.glsl>

#include <array>
#include <memory>

namespace MSG {
class Material;
struct MaterialExtensionBase;
struct MaterialExtensionSpecularGlossiness;
struct MaterialExtensionMetallicRoughness;
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
    void Set(Renderer::Impl& a_Renderer, const MSG::Material& a_SGMaterial);
    int type         = MATERIAL_TYPE_UNKNOWN;
    int alphaMode    = -1;
    bool doubleSided = false;
    bool unlit       = false;
    std::array<TextureSampler, SAMPLERS_MATERIAL_COUNT> textureSamplers;

private:
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
