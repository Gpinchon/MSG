#ifndef SAMPLE_SHADOW_MAP_GLSL
#define SAMPLE_SHADOW_MAP_GLSL
#ifndef __cplusplus

#include <Random.glsl>
#include <Types.glsl>

#if SHADOW_QUALITY == 1
#define SHADOW_SAMPLES 1
#elif SHADOW_QUALITY == 2
#define SHADOW_SAMPLES 4
#elif SHADOW_QUALITY == 3
#define SHADOW_SAMPLES 8
#else // SHADOW_QUALITY == 4
#define SHADOW_SAMPLES 16
#endif

struct ShadowData {
    float blurRadius;
    float minDepth;
    float maxDepth;
    vec3 lightPosition;
    vec3 surfacePosition;
    mat4 projection;
    mat4 view;
};

struct ShadowPointData {
    float blurRadius;
    float minDepth;
    float maxDepth;
    vec3 lightPosition;
    vec3 surfacePosition;
    float near;
    float far;
};

vec3 SampleHemisphere_Uniform(IN(uint) i, IN(uint) numSamples, IN(uvec2) a_RandBase)
{
    // Returns a 3D sample vector orientated around (0.0, 1.0, 0.0)
    // For practical use, must rotate with a rotation matrix (or whatever
    // your preferred approach is) for use with normals, etc.
    vec2 xi        = Hammersley16(i, numSamples, a_RandBase);
    float phi      = xi.y * 2.0 * M_PI;
    float cosTheta = 1.0 - xi.x;
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    return vec3(cos(phi) * sinTheta, cosTheta, sin(phi) * sinTheta);
}

float SampleShadowMap(IN(uint64_t) a_SamplerHandle, IN(ShadowPointData) a_Data, IN(vec2) a_FragCoord, IN(uint) a_FrameIndex)
{
    float shadow              = 0;
    vec3 lightDir             = normalize(a_Data.surfacePosition - a_Data.lightPosition);
    float lightDist           = normalizeValue(distance(a_Data.lightPosition, a_Data.surfacePosition), a_Data.near, a_Data.far);
    lightDist                 = normalizeValue(lightDist, a_Data.minDepth, a_Data.maxDepth);
    samplerCubeShadow sampler = samplerCubeShadow(a_SamplerHandle);
#if SHADOW_SAMPLES == 1
    shadow = texture(sampler, vec4(lightDir, lightDist));
#else
    const uvec2 rand = Rand3DPCG16(ivec3(a_FragCoord, a_FrameIndex)).xy;
    for (uint i = 0; i < SHADOW_SAMPLES; i++) {
        vec3 sampleVec = lightDir + SampleHemisphere_Uniform(i, SHADOW_SAMPLES, rand) * a_Data.blurRadius / 100.f;
        shadow += texture(sampler, vec4(sampleVec, lightDist));
    }
#endif
    return shadow / float(SHADOW_SAMPLES);
}

float SampleShadowMap(IN(uint64_t) a_SamplerHandle, IN(ShadowData) a_Data, IN(vec2) a_FragCoord, IN(uint) a_FrameIndex)
{
    const vec4 viewPos           = a_Data.view * vec4(a_Data.surfacePosition, 1.0);
    const vec4 shadowPos         = a_Data.projection * viewPos;
    const vec2 shadowCoord       = (shadowPos.xy / shadowPos.w) * 0.5 + 0.5;
    const int layerIndex         = 0;
    float lightDist              = shadowPos.z / shadowPos.w * 0.5 + 0.5;
    lightDist                    = normalizeValue(lightDist, a_Data.minDepth, a_Data.maxDepth);
    sampler2DArrayShadow sampler = sampler2DArrayShadow(a_SamplerHandle);
    float shadow                 = 0;
#if SHADOW_SAMPLES == 1
    const vec4 coords = vec4(shadowCoord.xy, layerIndex, lightDist);
    shadow            = texture(sampler, coords);
#else
    const vec2 texelSize = 1.f / vec2(textureSize(sampler, 0).xy);
    const uvec2 rand     = Rand3DPCG16(ivec3(a_FragCoord, a_FrameIndex)).xy;
    for (uint i = 0; i < SHADOW_SAMPLES; i++) {
        const vec2 offset = Hammersley16(i, SHADOW_SAMPLES, rand).xy * texelSize * a_Data.blurRadius;
        const vec4 coords = vec4(shadowCoord.xy + offset, layerIndex, lightDist);
        shadow += texture(sampler, coords);
    }
#endif
    return shadow / float(SHADOW_SAMPLES);
}

#endif //__cplusplus
#endif // SAMPLE_SHADOW_MAP_GLSL