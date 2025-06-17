#pragma once

#include <MSG/OGLTypedBuffer.hpp>
#include <MSG/Renderer/Handles.hpp>

#include <Bindings.glsl>
#include <Material.glsl>

#include <array>
#include <memory>

namespace MSG::Renderer {
class VirtualTexture;
}

namespace MSG {
class Material;
struct MaterialExtensionBase;
struct MaterialExtensionSpecularGlossiness;
struct MaterialExtensionMetallicRoughness;
class OGLSampler;
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
    std::shared_ptr<VirtualTexture> texture;
    std::shared_ptr<OGLSampler> sampler;
};

class Material {
public:
    Material(OGLContext& a_Context)
        : buffer(std::make_shared<OGLTypedBuffer<MaterialUBO>>(a_Context)) { };
    void Set(Renderer::Impl& a_Renderer, const MSG::Material& a_SGMaterial);
    int type         = MATERIAL_TYPE_UNKNOWN;
    int alphaMode    = -1;
    bool doubleSided = false;
    bool unlit       = false;
    std::shared_ptr<OGLTypedBuffer<MaterialUBO>> buffer;
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
