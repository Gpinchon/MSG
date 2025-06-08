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

struct ShadowDirData {
    float blurRadius;
    float minDepth;
    float maxDepth;
    vec3 lightPosition;
    vec3 surfacePosition;
    mat4 projection;
    mat4 view;
};

struct ShadowSpotData {
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

vec3 CubemapSampleDirToUVW(IN(vec3) a_UVW)
{
    vec3 vAbs = abs(a_UVW);
    float ma;
    vec2 uv;
    float faceIndex;
    if (vAbs.z >= vAbs.x && vAbs.z >= vAbs.y) {
        faceIndex = a_UVW.z < 0.f ? 5.f : 4.f;
        ma        = 0.5f / vAbs.z;
        uv        = vec2(a_UVW.z < 0.0 ? -a_UVW.x : a_UVW.x, -a_UVW.y);
    } else if (vAbs.y >= vAbs.x) {
        faceIndex = a_UVW.y < 0.f ? 3.f : 2.f;
        ma        = 0.5f / vAbs.y;
        uv        = vec2(a_UVW.x, a_UVW.y < 0.f ? -a_UVW.z : a_UVW.z);
    } else {
        faceIndex = a_UVW.x < 0.f ? 1.f : 0.f;
        ma        = 0.5f / vAbs.x;
        uv        = vec2(a_UVW.x < 0.f ? a_UVW.z : -a_UVW.z, -a_UVW.y);
    }
    return vec3(uv * ma + 0.5f, faceIndex);
}

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

float SampleShadowMap(
    IN(sampler2DArrayShadow) a_Sampler,
    IN(vec2) a_FragCoord,
    IN(uint) a_FrameIndex,
    IN(float) a_BlurRadius,
    IN(vec2) a_ShadowCoord,
    IN(int) a_ViewportIndex,
    IN(float) a_LightDistance)
{
    float shadow = 0;
#if SHADOW_SAMPLES == 1
    const vec4 coords = vec4(a_ShadowCoord, a_ViewportIndex, a_LightDistance);
    shadow            = texture(a_Sampler, coords);
#else
    const vec2 texelSize = 1.f / vec2(textureSize(a_Sampler, 0).xy);
    const uvec2 rand     = Rand3DPCG16(ivec3(a_FragCoord, a_FrameIndex)).xy;
    for (uint i = 0; i < SHADOW_SAMPLES; i++) {
        const vec2 offset = Hammersley16(i, SHADOW_SAMPLES, rand).xy * texelSize * a_BlurRadius;
        const vec4 coords = vec4(a_ShadowCoord + offset, a_ViewportIndex, a_LightDistance);
        shadow += texture(a_Sampler, coords);
    }
#endif
    return shadow / float(SHADOW_SAMPLES);
}

float SampleShadowMap(IN(sampler2DArrayShadow) a_Sampler, IN(ShadowDirData) a_Data, IN(vec2) a_FragCoord, IN(uint) a_FrameIndex)
{
    const vec4 viewPos     = a_Data.view * vec4(a_Data.surfacePosition, 1.0);
    const vec4 shadowPos   = a_Data.projection * viewPos;
    const vec2 shadowCoord = (shadowPos.xy / shadowPos.w) * 0.5 + 0.5;
    const int layerIndex   = 0;
    float lightDist        = shadowPos.z / shadowPos.w * 0.5 + 0.5;
    lightDist              = normalizeValue(lightDist, a_Data.minDepth, a_Data.maxDepth);
    return SampleShadowMap(
        a_Sampler,
        a_FragCoord, a_FrameIndex,
        a_Data.blurRadius,
        shadowCoord.xy, layerIndex,
        lightDist);
}

float SampleShadowMap(IN(sampler2DArrayShadow) a_Sampler, IN(ShadowSpotData) a_Data, IN(vec2) a_FragCoord, IN(uint) a_FrameIndex)
{
    const vec4 viewPos     = a_Data.view * vec4(a_Data.surfacePosition, 1.0);
    const vec4 shadowPos   = a_Data.projection * viewPos;
    const vec2 shadowCoord = (shadowPos.xy / shadowPos.w) * 0.5 + 0.5;
    const int layerIndex   = 0;
    float lightDist        = shadowPos.z / shadowPos.w * 0.5 + 0.5;
    lightDist              = normalizeValue(lightDist, a_Data.minDepth, a_Data.maxDepth);
    return SampleShadowMap(
        a_Sampler,
        a_FragCoord, a_FrameIndex,
        a_Data.blurRadius,
        shadowCoord, layerIndex,
        lightDist);
}

float SampleShadowMap(IN(sampler2DArrayShadow) a_Sampler, IN(ShadowPointData) a_Data, IN(vec2) a_FragCoord, IN(uint) a_FrameIndex)
{
    float shadow        = 0;
    const vec3 lightDir = normalize(a_Data.surfacePosition - a_Data.lightPosition);
    float lightDist     = normalizeValue(distance(a_Data.lightPosition, a_Data.surfacePosition), a_Data.near, a_Data.far);
    lightDist           = normalizeValue(lightDist, a_Data.minDepth, a_Data.maxDepth);
#if SHADOW_SAMPLES == 1
    vec3 uvw = CubemapSampleDirToUVW(lightDir);
    shadow   = texture(a_Sampler, vec4(uvw, lightDist));
#else
    const uvec2 rand = Rand3DPCG16(ivec3(a_FragCoord, a_FrameIndex)).xy;
    for (uint i = 0; i < SHADOW_SAMPLES; i++) {
        vec3 sampleVec = lightDir + SampleHemisphere_Uniform(i, SHADOW_SAMPLES, rand) * a_Data.blurRadius / 100.f;
        vec3 uvw       = CubemapSampleDirToUVW(sampleVec);
        shadow += texture(a_Sampler, vec4(uvw, lightDist));
    }
#endif
    return shadow / float(SHADOW_SAMPLES);
}

#endif //__cplusplus
#endif // SAMPLE_SHADOW_MAP_GLSL