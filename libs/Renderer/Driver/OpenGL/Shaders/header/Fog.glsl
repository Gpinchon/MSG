#ifndef FOG_GLSL
#define FOG_GLSL

#include <Functions.glsl>

#define FOG_WIDTH      128
#define FOG_HEIGHT     128
#define FOG_DEPTH      128
#define FOG_DEPTH_EXP  5.f
#define FOG_WORKGROUPS 16

#if FOG_QUALITY == 1
#define FOG_STEPS 16
#elif FOG_QUALITY == 2
#define FOG_STEPS 24
#elif FOG_QUALITY == 3
#define FOG_STEPS 32
#else // FOG_QUALITY == 4
#define FOG_STEPS 64
#endif

#define FOG_DENSITY_WIDTH  32
#define FOG_DENSITY_HEIGHT 32
#define FOG_DENSITY_DEPTH  32

#ifdef __cplusplus
#include <Types.glsl>
namespace MSG::Renderer::GLSL {
#endif //__cplusplus
struct FogSettings {
    vec3 noiseDensityOffset;
    float noiseDensityScale;
    float noiseDensityIntensity;
    float multiplier;
    uint _padding[2];
};

INLINE vec3 FogNDCFromUVW(IN(vec3) a_UVW)
{
    return vec3(a_UVW.x, a_UVW.y, pow(a_UVW.z, 1.f / FOG_DEPTH_EXP)) * 2.f - 1.f;
}

INLINE vec3 FogUVWFromNDC(IN(vec3) a_NDCPosition)
{
    vec3 uvw = a_NDCPosition * 0.5f + 0.5f;
    uvw.z    = pow(uvw.z, FOG_DEPTH_EXP);
    return uvw;
}

INLINE ivec3 FogTexCoordFromNDC(IN(vec3) a_NDCPosition, IN(vec3) a_TexSize)
{
    vec3 uvz = FogUVWFromNDC(a_NDCPosition);
    return ivec3(uvz * a_TexSize);
}

#ifdef __cplusplus
static_assert(sizeof(FogSettings) % 16 == 0);
static_assert(FOG_WIDTH % FOG_WORKGROUPS == 0);
static_assert(FOG_HEIGHT % FOG_WORKGROUPS == 0);
static_assert(FOG_DEPTH % FOG_WORKGROUPS == 0);
}
#endif //__cplusplus
#endif // FOG_GLSL