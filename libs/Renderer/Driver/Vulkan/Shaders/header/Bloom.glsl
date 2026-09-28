#ifndef BLOOM_GLSL
#define BLOOM_GLSL
#ifdef __cplusplus
#include <Types.glsl>
namespace Msg::Renderer::GLSL {
#endif //__cplusplus
struct BloomSettings {
    float intensity;
    float threshold;
    float smoothing;
    float size;
    vec3 tint;
    uint _padding[1];
};
#ifdef __cplusplus
}
#endif //__cplusplus
#endif // BLOOM_GLSL