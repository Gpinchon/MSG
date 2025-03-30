#ifndef FOG_GLSL
#define FOG_GLSL
#define FOG_WIDTH      64
#define FOG_HEIGHT     64
#define FOG_DEPTH      128
#define FOG_DEPTH_EXP  5.f
#define FOG_WORKGROUPS 8

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
    float transmittanceExp;
    uint _padding[1];
};
#ifdef __cplusplus
static_assert(sizeof(FogSettings) % 16 == 0);
static_assert(FOG_WIDTH % FOG_WORKGROUPS == 0);
static_assert(FOG_HEIGHT % FOG_WORKGROUPS == 0);
static_assert(FOG_DEPTH % FOG_WORKGROUPS == 0);
}
#endif //__cplusplus
#endif // FOG_GLSL