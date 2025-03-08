#ifndef SAMPLE_SHADOW_MAP_GLSL
#define SAMPLE_SHADOW_MAP_GLSL
#ifndef __cplusplus

#include <Random.glsl>
#include <Types.glsl>

#if SHADOW_QUALITY == 1
#define SHADOW_SAMPLES 1
#elif SHADOW_QUALITY == 2
#define SHADOW_SAMPLES 3
#elif SHADOW_QUALITY == 3
#define SHADOW_SAMPLES 5
#else // SHADOW_QUALITY == 4
#define SHADOW_SAMPLES 9
#endif

struct ShadowDirData {
    vec3 surfacePosition;
    mat4 projection;
    float near;
    float far;
    float blurRadius;
    ivec3 randBase;
};

struct ShadowSpotData {
    vec3 surfacePosition;
    mat4 projection;
    float near;
    float far;
    float blurRadius;
    ivec3 randBase;
};

struct ShadowPointData {
    vec3 lightDir; // L vector
    float lightDist; // distance from surface to light
    float near;
    float far;
    float blurRadius;
    ivec3 randBase;
};

/**
 * @brief Adaptive Depth Bias for Soft Shadows by Alexander Ehm, Alexander Ederer, Andreas Klein and Alfred Nischwitz
 *
 * @see https://w3-o.cs.hm.edu/users/nischwit/public_html/AdaptiveDepthBias_WSCG.pdf
 */
float ShadowBias(IN(float) a_Depth, IN(float) a_Near, IN(float) a_Far)
{
    const float K          = 0.1f;
    const float depthRange = a_Far - a_Near;
    return pow(a_Far - a_Depth * depthRange, 2.f) / (a_Far * a_Near * depthRange) * K;
}

float SampleShadowMap(
    IN(float) a_BlurRadius,
    IN(float) a_Bias,
    IN(vec3) a_ShadowCoord,
    IN(int) a_ViewportIndex,
    IN(ivec3) a_RandBase,
    sampler2DArrayShadow a_Sampler)
{
    const uvec2 random = Rand3DPCG16(a_RandBase).xy;
    float shadow       = 0;
    for (int i = 0; i < SHADOW_SAMPLES; i++) {
        vec2 sampleUV = a_ShadowCoord.xy + Hammersley(i, SHADOW_SAMPLES, random) * a_BlurRadius;
        shadow += texture(a_Sampler, vec4(sampleUV, a_ViewportIndex, a_ShadowCoord.z - a_Bias));
    }
    return (shadow / float(SHADOW_SAMPLES));
}

float SampleShadowMap(IN(sampler2DArrayShadow) a_Sampler, IN(ShadowDirData) a_Data)
{
    const vec4 shadowPos   = a_Data.projection * vec4(a_Data.surfacePosition, 1.0);
    const vec3 shadowCoord = vec3(shadowPos.xyz / shadowPos.w) * 0.5 + 0.5;
    const float shadowBias = 0.1f / (a_Data.far - a_Data.near);
    return SampleShadowMap(
        a_Data.blurRadius,
        shadowBias,
        shadowCoord,
        0, // layer index
        a_Data.randBase,
        a_Sampler);
}

float SampleShadowMap(IN(sampler2DArrayShadow) a_Sampler, IN(ShadowSpotData) a_Data)
{
    const vec4 shadowPos   = a_Data.projection * vec4(a_Data.surfacePosition, 1.0);
    const vec3 shadowCoord = vec3(shadowPos.xyz / shadowPos.w) * 0.5 + 0.5;
    const float shadowBias = ShadowBias(shadowCoord.z, a_Data.near, a_Data.far);
    return SampleShadowMap(
        a_Data.blurRadius,
        shadowBias,
        shadowCoord,
        0, // layer index
        a_Data.randBase,
        a_Sampler);
}

vec3 SampleHemisphere_Uniform(IN(uint) i, IN(uint) numSamples, IN(uvec2) a_RandBase)
{
    // Returns a 3D sample vector orientated around (0.0, 1.0, 0.0)
    // For practical use, must rotate with a rotation matrix (or whatever
    // your preferred approach is) for use with normals, etc.
    vec2 xi        = Hammersley(i, numSamples, a_RandBase);
    float phi      = xi.y * 2.0 * M_PI;
    float cosTheta = 1.0 - xi.x;
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    return vec3(cos(phi) * sinTheta, cosTheta, sin(phi) * sinTheta);
}

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

float SampleShadowMap(IN(sampler2DArrayShadow) a_Sampler, IN(ShadowPointData) a_Data)
{
    const float depth  = remap(a_Data.lightDist, a_Data.near, a_Data.far, 0, 1);
    const float bias   = 0.1f / (a_Data.far - a_Data.near);
    const uvec2 random = Rand3DPCG16(a_Data.randBase).xy;
    float shadow       = 0;
    for (int i = 0; i < SHADOW_SAMPLES; i++) {
        vec3 sampleVec = a_Data.lightDir + SampleHemisphere_Uniform(i, SHADOW_SAMPLES, random) * a_Data.blurRadius;
        vec3 uvw       = CubemapSampleDirToUVW(sampleVec);
        shadow += texture(a_Sampler, vec4(uvw, depth - bias));
    }
    return shadow / float(SHADOW_SAMPLES);
}

#endif //__cplusplus
#endif // SAMPLE_SHADOW_MAP_GLSL