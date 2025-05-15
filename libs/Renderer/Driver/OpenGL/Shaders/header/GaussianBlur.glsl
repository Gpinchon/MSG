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
#ifndef __cplusplus
#include <Functions.glsl>

#define GAUSSIAN_SAMPLES 9
const float GaussianBlurWeights[9] = float[9](0.05, 0.09, 0.12, 0.15, 0.16, 0.15, 0.12, 0.09, 0.05);

vec4 GaussianBlur(IN(sampler2D) a_Sampler, IN(vec2) a_UV, IN(GaussianBlurSettings) a_Settings)
{
    const vec2 texelSize = vec2(1.f / textureSize(a_Sampler, 0).xy);
    const vec2 stepSize  = (texelSize * a_Settings.direction * a_Settings.scale) / float(GAUSSIAN_SAMPLES / 2);
    vec2 uv              = a_UV - (stepSize * float(GAUSSIAN_SAMPLES / 2));
    vec4 outColor        = vec4(0);
    for (int i = 0; i < GAUSSIAN_SAMPLES; ++i) {
        outColor += texture(a_Sampler, uv) * GaussianBlurWeights[i];
        uv += stepSize;
    }
    return outColor;
}
#endif
#ifdef __cplusplus
struct GaussianBlurSettingsUBO : GaussianBlurSettings {
    uint _padding[60];
};
static_assert(sizeof(GaussianBlurSettingsUBO) % 16 == 0);
static_assert(sizeof(GaussianBlurSettingsUBO) % 256 == 0);
}
#endif //__cplusplus

#endif // GAUSSIAN_BLUR_GLSL
