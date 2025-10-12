#ifndef VIRTUAL_TEXTURING_GLSL
#define VIRTUAL_TEXTURING_GLSL

#include <Bindings.glsl>
#include <Material.glsl>

#define VT_WRAP_CLAMP         0
#define VT_WRAP_CLAMP_MIRROR  1
#define VT_WRAP_REPEAT        2
#define VT_WRAP_REPEAT_MIRROR 3

#ifdef __cplusplus
#include <Types.glsl>
namespace Msg::Renderer::GLSL {
#endif
struct VTFeedbackOutput {
    vec2 minUV;
    vec2 maxUV;
    float minMip;
    float maxMip;
    uint _padding[2];
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
float VTComputeLOD(IN(vec2) a_TexCoord)
{
    vec2 dx  = dFdx(a_TexCoord);
    vec2 dy  = dFdy(a_TexCoord);
    float px = dot(dx, dx);
    float py = dot(dy, dy);
    float p  = max(px, py);
    return max(0.5f * log2(p), 0.f);
}

float VTComputeLOD(IN(vec2) a_TexCoord, IN(float) a_MaxAniso)
{
    if (a_MaxAniso == 0)
        return VTComputeLOD(a_TexCoord);
    float maxAnisoLog2 = log2(a_MaxAniso);
    vec2 dx            = dFdx(a_TexCoord);
    vec2 dy            = dFdy(a_TexCoord);
    float px           = dot(dx, dx);
    float py           = dot(dy, dy);
    float pMax         = max(px, py);
    float pMin         = min(px, py);
    float N            = max(min(ceil(pMax / pMin), maxAnisoLog2), 1.f);
    float p            = pMax / N;
    return max(0.5f * log2(p), 0.f);
}
#endif
#ifdef __cplusplus
static_assert(sizeof(VTFeedbackOutput) % 16 == 0);
static_assert(sizeof(VTTextureInfo) % 16 == 0);
static_assert(sizeof(VTMaterialInfo) % 16 == 0);
}
#endif
#endif // VIRTUAL_TEXTURING_GLSL