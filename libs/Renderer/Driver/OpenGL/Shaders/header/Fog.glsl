#ifndef FOG_GLSL
#define FOG_GLSL
#define FOG_WIDTH      64
#define FOG_HEIGHT     64
#define FOG_DEPTH      96
#define FOG_DEPTH_EXP  5.f
#define FOG_STEPS      32
#define FOG_WORKGROUPS 8

#define FOG_DENSITY_WIDTH  32
#define FOG_DENSITY_HEIGHT 32
#define FOG_DENSITY_DEPTH  32

#ifdef __cplusplus
#include <Types.glsl>
namespace MSG::Renderer::GLSL {
#endif //__cplusplus
struct FogSettings {
    vec3 noiseDensityOffset;
    float noiseDensityIntensity;
    float noiseDensityScale;
    float noiseDepthMultiplier;
    float multiplier;
    float attenuationExp;
};
#ifdef __cplusplus
static_assert(sizeof(FogSettings) % 16 == 0);
static_assert(FOG_WIDTH % FOG_WORKGROUPS == 0);
static_assert(FOG_HEIGHT % FOG_WORKGROUPS == 0);
static_assert(FOG_DEPTH % FOG_WORKGROUPS == 0);
}
#endif //__cplusplus
#endif // FOG_GLSL