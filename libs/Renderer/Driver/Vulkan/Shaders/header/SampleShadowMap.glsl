#ifndef SAMPLE_SHADOW_MAP_GLSL
#define SAMPLE_SHADOW_MAP_GLSL
#ifndef __cplusplus

#include <Random.glsl>
#include <Types.glsl>

#if SHADOW_QUALITY == 1
#define SHADOW_OCCLUDER_SAMPLES 1
#define SHADOW_SAMPLES          1
#elif SHADOW_QUALITY == 2
#define SHADOW_OCCLUDER_SAMPLES 4
#define SHADOW_SAMPLES          4
#elif SHADOW_QUALITY == 3
#define SHADOW_OCCLUDER_SAMPLES 4
#define SHADOW_SAMPLES          8
#else // SHADOW_QUALITY == 4
#define SHADOW_OCCLUDER_SAMPLES 8
#define SHADOW_SAMPLES          16
#endif

struct ShadowData {
    float blurRadius;
    float pcssBlurRadius;
    float minDepth;
    float maxDepth;
    vec3 lightPosition;
    vec3 surfacePosition;
    mat4 projection;
    mat4 view;
};

struct ShadowPointData {
    float blurRadius;
    float pcssBlurRadius;
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

float SampleShadowMap(IN(sampler2DArray) a_Sampler, IN(vec3) a_UVW)
{
    vec2 texSize       = textureSize(a_Sampler, 0).xy;
    vec2 st            = a_UVW.xy * texSize - 0.5f;
    vec2 weights       = saturate(fract(st)); // necessary on NVidia for some reason
    vec4 shadowSamples = textureGather(a_Sampler, a_UVW);
    for (int sampleI = 0; sampleI < 4; sampleI++) {
        float shadowVal = shadowSamples[sampleI];
        if (shadowVal > 0 && shadowVal < 1)
            shadowSamples[sampleI] = shadowVal;
        else
            shadowSamples[sampleI] = 1;
    }
    return mix(
        mix(shadowSamples[3], shadowSamples[2], weights.x),
        mix(shadowSamples[0], shadowSamples[1], weights.x),
        weights.y);
}

float SampleShadowMapPCF(IN(sampler2DArray) a_Sampler, IN(vec3) a_UVW, IN(float) a_Comp)
{
    vec2 texSize       = textureSize(a_Sampler, 0).xy;
    vec2 st            = a_UVW.xy * texSize - 0.5f;
    vec2 weights       = saturate(fract(st)); // necessary on NVidia for some reason
    vec4 shadowSamples = textureGather(a_Sampler, a_UVW);
    for (int sampleI = 0; sampleI < 4; sampleI++) {
        float shadowVal = shadowSamples[sampleI];
        if (shadowVal > 0 && shadowVal < 1)
            shadowSamples[sampleI] = step(a_Comp, shadowVal);
        else
            shadowSamples[sampleI] = 1;
    }
    return mix(
        mix(shadowSamples[3], shadowSamples[2], weights.x),
        mix(shadowSamples[0], shadowSamples[1], weights.x),
        weights.y);
}

float ShadowMapFindOccluder(IN(sampler2DArray) a_Sampler, IN(float) a_BlurRadius, IN(uvec2) a_Rand, IN(vec3) a_UVW, IN(float) a_ReceiverDist)
{
    vec2 texelSize     = 1.f / vec2(textureSize(a_Sampler, 0).xy);
    vec2 searchWidth   = texelSize * a_BlurRadius;
    float numOccluders = 0;
    float occluderSum  = 0;
    for (int i = 0; i < SHADOW_OCCLUDER_SAMPLES; i++) {
        vec2 offset     = Hammersley16(i, SHADOW_OCCLUDER_SAMPLES, a_Rand).xy * searchWidth;
        vec3 coords     = vec3(a_UVW.xy + offset, a_UVW.z);
        float shadowVal = SampleShadowMap(a_Sampler, coords);
        if (shadowVal < a_ReceiverDist) {
            numOccluders++;
            occluderSum += shadowVal;
        }
    }
    return numOccluders > 0 ? occluderSum / numOccluders : 0;
}

float ShadowMapFindOccluder(IN(samplerCube) a_Sampler, IN(float) a_BlurRadius, IN(uvec2) a_Rand, IN(vec3) a_Vec, IN(float) a_ReceiverDist)
{
    vec2 texelSize     = 1.f / vec2(textureSize(a_Sampler, 0).xy);
    float searchWidth  = texelSize.x * a_BlurRadius;
    float numOccluders = 0;
    float occluderSum  = 0;
    for (int i = 0; i < SHADOW_OCCLUDER_SAMPLES; i++) {
        vec3 sampleVec  = a_Vec + SampleHemisphere_Uniform(i, SHADOW_SAMPLES, a_Rand) * searchWidth;
        float shadowVal = texture(a_Sampler, sampleVec)[0];
        if (shadowVal < a_ReceiverDist) {
            numOccluders++;
            occluderSum += shadowVal;
        }
    }
    return numOccluders > 0 ? occluderSum / numOccluders : 0;
}

float SampleShadowMap(IN(uint64_t) a_SamplerHandle, IN(ShadowPointData) a_Data, IN(vec2) a_FragCoord, IN(uint) a_FrameIndex)
{
    float shadow        = 0;
    vec3 lightDir       = normalize(a_Data.surfacePosition - a_Data.lightPosition);
    float receiverDist  = normalizeValue(distance(a_Data.lightPosition, a_Data.surfacePosition), a_Data.near, a_Data.far);
    receiverDist        = normalizeValue(receiverDist, a_Data.minDepth, a_Data.maxDepth);
    samplerCube sampler = samplerCube(unpackUint2x32(a_SamplerHandle));
    vec2 texelSize      = 1.f / vec2(textureSize(sampler, 0).xy);
#if SHADOW_SAMPLES == 1
    return texture(sampler, lightDir)[0] > receiverDist ? 1 : 0;
#else
    const uvec2 rand = Rand3DPCG16(ivec3(a_FragCoord, a_FrameIndex)).xy;
#if SHADOW_ENABLE_PCSS
    float occluderDist = ShadowMapFindOccluder(sampler, a_Data.pcssBlurRadius, rand, lightDir, receiverDist);
    if (occluderDist == 0)
        return 1; // occluder is behind receiver, no shadow here
    float penumbra   = a_Data.blurRadius + a_Data.pcssBlurRadius * saturate((receiverDist - occluderDist) / occluderDist);
    float blurRadius = penumbra * texelSize.x;
#else
    float blurRadius = a_Data.blurRadius * texelSize.x;
#endif
    for (uint i = 0; i < SHADOW_SAMPLES; i++) {
        vec3 sampleVec  = lightDir + SampleHemisphere_Uniform(i, SHADOW_SAMPLES, rand) * blurRadius;
        float shadowVal = texture(sampler, sampleVec)[0];
        shadow += shadowVal > receiverDist ? 1 : 0;
    }
    return shadow / float(SHADOW_SAMPLES);
#endif
}

float SampleShadowMap(IN(uint64_t) a_SamplerHandle, IN(ShadowData) a_Data, IN(vec2) a_FragCoord, IN(uint) a_FrameIndex)
{
    sampler2DArray sampler = sampler2DArray(unpackUint2x32(a_SamplerHandle));
    vec4 viewPos           = a_Data.view * vec4(a_Data.surfacePosition, 1.0);
    vec4 shadowPos         = a_Data.projection * viewPos;
    vec3 shadowCoord       = vec3((shadowPos.xy / shadowPos.w) * 0.5 + 0.5, 0);
    float receiverDist     = normalizeValue(shadowPos.z / shadowPos.w * 0.5 + 0.5, a_Data.minDepth, a_Data.maxDepth);
#if SHADOW_QUALITY == 1
    return SampleShadowMapPCF(sampler, shadowCoord, receiverDist);
#else
    vec2 texelSize = 1.f / vec2(textureSize(sampler, 0).xy);
    uvec2 rand     = Rand3DPCG16(ivec3(a_FragCoord, a_FrameIndex)).xy;
#if SHADOW_ENABLE_PCSS
    float occluderDist = ShadowMapFindOccluder(sampler, a_Data.pcssBlurRadius, rand, shadowCoord, receiverDist);
    if (occluderDist == 0)
        return 1; // occluder is behind receiver, no shadow here
    float penumbra  = a_Data.blurRadius + a_Data.pcssBlurRadius * saturate((receiverDist - occluderDist) / occluderDist);
    vec2 blurRadius = penumbra * texelSize;
#else
    vec2 blurRadius = a_Data.blurRadius * texelSize;
#endif
    float shadow = 0;
    for (uint i = 0; i < SHADOW_SAMPLES; i++) {
        vec2 offset = Hammersley16(i, SHADOW_SAMPLES, rand).xy * blurRadius;
        vec3 coords = vec3(shadowCoord.xy + offset, shadowCoord.z);
        shadow += SampleShadowMapPCF(sampler, coords, receiverDist);
    }
    return shadow / float(SHADOW_SAMPLES);
#endif
}

#endif //__cplusplus
#endif // SAMPLE_SHADOW_MAP_GLSL