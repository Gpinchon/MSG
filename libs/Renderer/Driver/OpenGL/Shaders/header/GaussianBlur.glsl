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
#include <Bicubic.glsl>

const float GaussianBlurWeights[5] = float[5](0.16, 0.15, 0.12, 0.09, 0.05);

vec4 GaussianBlur(IN(sampler2D) a_Sampler, IN(vec2) a_UV, IN(GaussianBlurSettings) a_Settings)
{
    const vec2 texelSize = vec2(1.f / textureSize(a_Sampler, 0).xy);
    const vec2 offset    = (texelSize * a_Settings.direction * a_Settings.scale) / float(GaussianBlurWeights.length());
    vec4 outColor        = textureNice(a_Sampler, a_UV) * GaussianBlurWeights[0];
    for (uint i = 1; i < GaussianBlurWeights.length(); ++i) {
        const vec2 UV0 = a_UV + (offset * float(i));
        const vec2 UV1 = a_UV - (offset * float(i));
        outColor += textureNice(a_Sampler, UV0) * GaussianBlurWeights[i];
        outColor += textureNice(a_Sampler, UV1) * GaussianBlurWeights[i];
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
