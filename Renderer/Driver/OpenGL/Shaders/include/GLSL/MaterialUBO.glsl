#ifndef MATERIAL_UBO
#define MATERIAL_UBO

#ifdef __cplusplus
#include <GLSL/Types.glsl>
namespace TabGraph::Renderer::GLSL {
#if false // this is a test
#else
#endif
#else //__cplusplus
#include <Types.glsl>
#if false // this is a test
#ifdef CACA
#else
#if
#endif
//WHEEE
#endif
#else
#endif
#endif //__cplusplus

struct TextureTransform {
#ifdef __cplusplus
    TextureTransform()
        : offset(0, 0)
        , scale(1, 1)
        , rotation(0)
    {
    }
#endif //__cplusplus
    VEC2 offset;
    VEC2 scale;
    float rotation;
    UINT _padding[3];
};

struct TextureInfo {
#ifdef __cplusplus
    TextureInfo()
        : handle(0)
        , texCoord(0)
    {
    }
#endif //__cplusplus
    TextureTransform transform;
    SAMPLER2D handle;
    UINT texCoord;
    UINT _padding[1];
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
    VEC3 colorFactor;
    float roughnessFactor;
    UINT _padding[1];
};

struct NormalTexture {
#ifdef __cplusplus
    NormalTexture()
        : scale(1)
    {
    }
#endif //__cplusplus
    TextureInfo info;
    float scale;
};

struct OcclusionTexture {
#ifdef __cplusplus
    OcclusionTexture()
        : strength(1)
    {
    }
#endif //__cplusplus
    TextureInfo info;
    float strength;
};

struct BaseMaterial {
#ifdef __cplusplus
    BaseMaterial()
        : emissiveFactor(1, 1, 1)
        , alphaCutoff(0.5)
    {
    }
#endif //__cplusplus
    NormalTexture normalTexture;
    OcclusionTexture occlusionTexture;
    TextureInfo emissiveTexture;
    VEC3 emissiveFactor;
    float alphaCutoff;
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
    TextureInfo diffuseTexture;
    TextureInfo specularGlossinessTexture;
    VEC4 diffuseFactor;
    VEC3 specularFactor;
    float glossinessFactor;
    UINT _padding[2];
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
    TextureInfo colorTexture;
    TextureInfo metallicRoughnessTexture;
    VEC4 colorFactor;
    float metallicFactor;
    float roughnessFactor;
    UINT _padding[2];
};

#ifdef __cplusplus
}
#endif //__cplusplus

#endif
