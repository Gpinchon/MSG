#ifndef VIRTUAL_TEXTURING_GLSL
#define VIRTUAL_TEXTURING_GLSL

#include <Bindings.glsl>
#include <Functions.glsl>
#include <Material.glsl>

#define VT_WRAP_CLAMP         0
#define VT_WRAP_CLAMP_MIRROR  1
#define VT_WRAP_REPEAT        2
#define VT_WRAP_REPEAT_MIRROR 3

#ifdef __cplusplus
#include <Types.glsl>
namespace Msg::Renderer::GLSL {
#endif
struct VTFeedbackSettings {
    vec2 bufferRatio;
    float maxAnisotropy;
    uint _padding[1];
};

struct VTTextureInfo {
    TextureTransform transform;
    uint texCoord;
    uint id;
    uint wrapS;
    uint wrapT;
    float maxAniso;
    float lodBias;
    vec2 texSize;
};

struct VTMaterialInfo {
    VTTextureInfo textures[SAMPLERS_MATERIAL_COUNT];
};

#ifndef __cplusplus
/**
 * @brief This computation should correspond to GL 2.0 specs equation 3.18
 * https://registry.khronos.org/OpenGL/specs/gl/glspec20.pdf
 *
 * @return the unclamped desired LOD
 */
float VTComputeLOD(IN(vec2) a_UV, IN(vec2) a_TexSize)
{
    vec2 dx  = dFdx(a_UV) * a_TexSize;
    vec2 dy  = dFdy(a_UV) * a_TexSize;
    float px = dot(dx, dx);
    float py = dot(dy, dy);
    float p  = max(px, py);
    return max(0.5f * log2(p), 0.f);
}

float VTComputeLOD(IN(vec2) a_TexCoord)
{
    return VTComputeLOD(a_TexCoord, vec2(1));
}

float VTComputeLOD(IN(vec2) a_UV, IN(vec2) a_TexSize, IN(float) a_MaxAniso)
{
    if (a_MaxAniso == 0)
        return VTComputeLOD(a_UV, a_TexSize);
    vec2 dx      = dFdx(a_UV) * a_TexSize;
    vec2 dy      = dFdy(a_UV) * a_TexSize;
    float px     = dot(dx, dx);
    float py     = dot(dy, dy);
    float pMax   = max(px, py);
    float pMin   = min(px, py);
    float pRatio = min(pMax / max(pMin, 0.0001f), a_MaxAniso * a_MaxAniso);
    return max(0.5f * log2(pMax / pRatio), 0.0f);
}

float VTComputeLOD(IN(vec2) a_TexCoord, IN(float) a_MaxAniso)
{
    return VTComputeLOD(a_TexCoord, vec2(1), a_MaxAniso);
}
#endif
INLINE float Mirror(IN(float) a_Val) { return a_Val >= 0.f ? a_Val : -(1.f + a_Val); }

INLINE float WrapTexelCoord(IN(uint) a_Wrap, IN(float) a_Size, IN(float) a_Coord)
{
    if (a_Wrap == VT_WRAP_REPEAT)
        return mod(a_Coord, a_Size); // handle negative indice as well
    else if (a_Wrap == VT_WRAP_CLAMP)
        return clamp(a_Coord, 0.f, a_Size - 1);
    else if (a_Wrap == VT_WRAP_REPEAT_MIRROR)
        return (a_Size - 1) - Mirror(mod(a_Coord, (2 * a_Size))) - a_Size;
    else if (a_Wrap == VT_WRAP_CLAMP_MIRROR)
        return clamp(Mirror(a_Coord), 0.f, a_Size - 1);
    else
        return a_Coord;
}

INLINE vec2 WrapTexelCoords(
    IN(uint) a_WrapS,
    IN(uint) a_WrapT,
    IN(vec2) a_TextureSize,
    IN(vec2) a_TexelCoord)
{
    return vec2(
        WrapTexelCoord(a_WrapS, a_TextureSize[0], a_TexelCoord[0]),
        WrapTexelCoord(a_WrapT, a_TextureSize[1], a_TexelCoord[1]));
}
#ifdef __cplusplus
static_assert(sizeof(VTTextureInfo) % 16 == 0);
static_assert(sizeof(VTMaterialInfo) % 16 == 0);
}
#endif
#endif // VIRTUAL_TEXTURING_GLSL