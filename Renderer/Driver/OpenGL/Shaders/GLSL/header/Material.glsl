#ifndef MATERIAL_GLSL
#define MATERIAL_GLSL
//////////////////////////////////////// INCLUDES
#include <Types.glsl>
//////////////////////////////////////// INCLUDES

#ifdef __cplusplus
namespace TabGraph::Renderer::GLSL {
#endif //__cplusplus
#define MATERIAL_TYPE_UNKNOWN             (-1)
#define MATERIAL_TYPE_METALLIC_ROUGHNESS  (MATERIAL_TYPE_UNKNOWN + 1)
#define MATERIAL_TYPE_SPECULAR_GLOSSINESS (MATERIAL_TYPE_METALLIC_ROUGHNESS + 1)

#define MATERIAL_ALPHA_OPAQUE (0)
#define MATERIAL_ALPHA_CUTOFF (1)
#define MATERIAL_ALPHA_BLEND  (2)

struct TextureTransform {
#ifdef __cplusplus
    TextureTransform()
        : offset(0, 0)
        , scale(1, 1)
        , rotation(0)
    {
    }
#endif //__cplusplus
    vec2 offset;
    vec2 scale;
    float rotation;
    uint _padding[3];
};

struct TextureInfo {
#ifdef __cplusplus
    TextureInfo()
        : texCoord(0)
    {
    }
#endif //__cplusplus
    TextureTransform transform;
    uint texCoord;
    uint _padding[3];
};

struct Sheen {
#ifdef __cplusplus
    Sheen()
        : colorFactor(1, 1, 1)
        , roughnessFactor(1)
    {
    }
#endif //__cplusplus
    TextureInfo colorTexture;
    TextureInfo roughnessTexture;
    vec3 colorFactor;
    float roughnessFactor;
};

struct BaseMaterial {
#ifdef __cplusplus
    BaseMaterial()
        : normalScale(1)
        , occlusionStrength(1)
        , emissiveFactor(1, 1, 1)
        , alphaCutoff(0.5)
    {
    }
#endif //__cplusplus
    vec3 emissiveFactor;
    float normalScale;
    float occlusionStrength;
    float alphaCutoff;
    uint _padding[2];
};

struct SpecularGlossinessMaterial {
#ifdef __cplusplus
    SpecularGlossinessMaterial()
        : diffuseFactor(1, 1, 1, 1)
        , specularFactor(1, 1, 1)
        , glossinessFactor(1)
    {
    }
#endif //__cplusplus
    BaseMaterial base;
    Sheen sheen;
    vec4 diffuseFactor;
    vec3 specularFactor;
    float glossinessFactor;
};

struct MetallicRoughnessMaterial {
#ifdef __cplusplus
    MetallicRoughnessMaterial()
        : colorFactor(1, 1, 1, 1)
        , metallicFactor(0.5)
        , roughnessFactor(0.5)
    {
    }
#endif //__cplusplus
    BaseMaterial base;
    Sheen sheen;
    vec4 colorFactor;
    float metallicFactor;
    float roughnessFactor;
    uint _padding[2];
};

struct CommonMaterial {
    BaseMaterial base;
    Sheen sheen;
    uint _padding[8];
};

#ifdef __cplusplus
static_assert(sizeof(TextureTransform) % 16 == 0);
static_assert(sizeof(TextureInfo) % 16 == 0);
static_assert(sizeof(Sheen) % 16 == 0);
static_assert(sizeof(BaseMaterial) % 16 == 0);
static_assert(sizeof(CommonMaterial) % 16 == 0);
static_assert(sizeof(CommonMaterial) == sizeof(SpecularGlossinessMaterial));
static_assert(sizeof(CommonMaterial) == sizeof(MetallicRoughnessMaterial));
}
#endif //__cplusplus

#endif // MATERIAL_GLSL
