#ifndef GAUSSIAN_BLUR_GLSL
#define GAUSSIAN_BLUR_GLSL

#include <Types.glsl>

#define GAUSSIAN_BLUR_PASS_COUNT 2

#ifdef __cplusplus
namespace MSG::Renderer::GLSL {
#endif //__cplusplus

struct GaussianBlurSettings {
    vec2 direction;
    vec2 scale;
};

#ifdef __cplusplus
struct GaussianBlurSettingsUBO : GaussianBlurSettings {
    uint _padding[60];
};
static_assert(sizeof(GaussianBlurSettingsUBO) % 16 == 0);
static_assert(sizeof(GaussianBlurSettingsUBO) % 256 == 0);
}
#endif //__cplusplus

#endif // GAUSSIAN_BLUR_GLSL
