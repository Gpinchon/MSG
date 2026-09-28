#ifndef KARIS_AVERAGE_GLSL
#define KARIS_AVERAGE_GLSL
#ifndef __cplusplus
#include <ToneMapping.glsl>
// Karis's luma weighted average
vec4 KarisAverageSample(IN(sampler3D) a_Src, IN(vec3) a_UV)
{
    const vec3 samples[9] = {
        vec3(-1, -1, -1),
        vec3(-1, 1, -1),
        vec3(1, -1, -1),
        vec3(1, 1, -1),
        vec3(0, 0, 0),
        vec3(-1, -1, 1),
        vec3(-1, 1, 1),
        vec3(1, -1, 1),
        vec3(1, 1, 1),
    };
    vec3 pixSize    = 1.f / textureSize(a_Src, 0);
    vec4 result     = vec4(0);
    float weightSum = 0.f;
    for (uint i = 0; i < samples.length(); i++) {
        vec3 offset  = pixSize * samples[i];
        vec4 color   = texture(a_Src, a_UV + offset);
        float weight = 1.f / (Luminance(color.rgb) + 1.f);
        result += color * weight;
        weightSum += weight;
    }
    return result / weightSum;
}

// Karis's luma weighted average
vec4 KarisAverageSample(IN(sampler2D) a_Src, IN(vec2) a_UV)
{
    const vec2 samples[5] = {
        vec2(-1, -1), /**********/ vec2(-1, 1),
        /***********/ vec2(0, 0), /**********/
        vec2(1, -1), /***********/ vec2(1, 1)
    };
    vec2 pixSize    = 1.f / textureSize(a_Src, 0);
    vec4 result     = vec4(0);
    float weightSum = 0.f;
    for (uint i = 0; i < samples.length(); i++) {
        vec2 offset  = pixSize * samples[i];
        vec4 color   = texture(a_Src, a_UV + offset);
        float weight = 1.f / (Luminance(color.rgb) + 1.f);
        result += color * weight;
        weightSum += weight;
    }
    return result / weightSum;
}
#endif //__cplusplus
#endif // KARIS_AVERAGE_GLSL