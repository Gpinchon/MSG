#ifndef FOG_GLSL
#define FOG_GLSL

#include <Functions.glsl>

#define FOG_WIDTH_LOW  96
#define FOG_HEIGHT_LOW 96
#define FOG_DEPTH_LOW  64

#define FOG_WIDTH_MID  128
#define FOG_HEIGHT_MID 128
#define FOG_DEPTH_MID  64

#define FOG_WIDTH_HIGH  128
#define FOG_HEIGHT_HIGH 128
#define FOG_DEPTH_HIGH  96

#define FOG_WIDTH_VHIGH  128
#define FOG_HEIGHT_VHIGH 128
#define FOG_DEPTH_VHIGH  128

#if FOG_QUALITY == 1
#define FOG_STEPS  16
#define FOG_WIDTH  FOG_WIDTH_LOW
#define FOG_HEIGHT FOG_HEIGHT_LOW
#define FOG_DEPTH  FOG_DEPTH_LOW
#elif FOG_QUALITY == 2
#define FOG_STEPS  24
#define FOG_WIDTH  FOG_WIDTH_MID
#define FOG_HEIGHT FOG_HEIGHT_MID
#define FOG_DEPTH  FOG_DEPTH_MID
#elif FOG_QUALITY == 3
#define FOG_STEPS  32
#define FOG_WIDTH  FOG_WIDTH_HIGH
#define FOG_HEIGHT FOG_HEIGHT_HIGH
#define FOG_DEPTH  FOG_DEPTH_HIGH
#else // FOG_QUALITY == 4
#define FOG_STEPS  64
#define FOG_WIDTH  FOG_WIDTH_VHIGH
#define FOG_HEIGHT FOG_HEIGHT_VHIGH
#define FOG_DEPTH  FOG_DEPTH_VHIGH
#endif

#define FOG_WORKGROUPS 16

#ifdef __cplusplus
#include <Types.glsl>
namespace MSG::Renderer::GLSL {
#endif //__cplusplus
struct FogSettings {
    vec3 globalScattering;
    float globalExtinction;
    vec3 globalEmissive;
    float globalPhaseG;
    vec3 noiseDensityOffset;
    float noiseDensityScale;
    float noiseDensityIntensity;
    float noiseDensityMaxDist;
    float depthExponant;
    uint _padding[1];
};

INLINE vec3 FogNDCFromUVW(IN(vec3) a_UVW, IN(float) a_Exponant)
{
    return vec3(a_UVW.x, a_UVW.y, pow(a_UVW.z, 1.f / a_Exponant)) * 2.f - 1.f;
}

INLINE vec3 FogUVWFromNDC(IN(vec3) a_NDCPosition, IN(float) a_Exponant)
{
    vec3 uvw = a_NDCPosition * 0.5f + 0.5f;
    uvw.z    = pow(uvw.z, a_Exponant);
    return uvw;
}

INLINE ivec3 FogTexCoordFromNDC(IN(vec3) a_NDCPosition, IN(float) a_Exponant, IN(vec3) a_TexSize)
{
    vec3 uvz = FogUVWFromNDC(a_NDCPosition, a_Exponant);
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