#ifndef FOG_CULLING_GLSL
#define FOG_CULLING_GLSL

#define FOG_WIDTH     128
#define FOG_HEIGHT    128
#define FOG_DEPTH     128
#define FOG_COUNT     (FOG_WIDTH * FOG_HEIGHT * FOG_DEPTH)
#define FOG_DEPTH_EXP (1 / 4.f)
#define FOG_STEPS     32

#define FOG_DENSITY_WIDTH  32
#define FOG_DENSITY_HEIGHT 32
#define FOG_DENSITY_DEPTH  32

#endif // FOG_CULLING_GLSL