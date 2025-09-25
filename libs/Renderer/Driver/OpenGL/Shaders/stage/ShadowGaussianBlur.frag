#include <Functions.glsl>
#include <GaussianBlur.glsl>

layout(binding = 0) uniform sampler2D u_Moments;

layout(location = 0) in invariant vec2 in_UV;

layout(location = 0) out float out_Depth;

layout(binding = 0) uniform SettingsBlock
{
    GaussianBlurSettings u_Settings;
};

#define FLT_MAX 3.40282347e+38

float ShadowGaussianBlur(IN(sampler2D) a_Sampler, IN(vec2) a_UV, IN(GaussianBlurSettings) a_Settings)
{
    const vec2 texelSize = vec2(1.f / textureSize(a_Sampler, 0).xy);
    const vec2 stepSize  = (texelSize * a_Settings.direction * a_Settings.scale) / float(GAUSSIAN_SAMPLES / 2);
    vec2 uv              = a_UV - (stepSize * float(GAUSSIAN_SAMPLES / 2));
    uint samples         = 0;
    float outDepth       = 0;
    float sampleWeight   = GaussianBlurWeights[0];
    for (int i = 0; i < GAUSSIAN_SAMPLES; ++i) {
        float depth = texture(a_Sampler, uv)[0];
        if (depth < FLT_MAX) { // avoid sampling empty shadow
            outDepth += depth * sampleWeight;
            sampleWeight = GaussianBlurWeights[i];
            samples++;
        } else {
            sampleWeight += GaussianBlurWeights[i];
        }
        uv += stepSize;
    }
    return samples == 0 ? FLT_MAX : outDepth;
}

void main()
{
    out_Depth = ShadowGaussianBlur(u_Moments, in_UV, u_Settings);
}
