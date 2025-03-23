#ifndef FOG_GLSL
#define FOG_GLSL
#define FOG_WIDTH     128
#define FOG_HEIGHT    128
#define FOG_DEPTH     128
#define FOG_COUNT     (FOG_WIDTH * FOG_HEIGHT * FOG_DEPTH)
#define FOG_DEPTH_EXP 5.f
#define FOG_STEPS     32

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
    float noiseDepthMultiplier;
    float multiplier;
    uint _padding[2];
};
#ifdef __cplusplus
static_assert(sizeof(FogSettings) % 16 == 0);
}
#endif //__cplusplus
#endif // FOG_GLSL