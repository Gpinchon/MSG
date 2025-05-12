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
    vec3 lightPosition;
    vec3 surfacePosition;
    mat4 projection;
    mat4 view;
};

struct ShadowSpotData {
    vec3 lightPosition;
    vec3 surfacePosition;
    mat4 projection;
    mat4 view;
};

struct ShadowPointData {
    vec3 lightPosition;
    vec3 surfacePosition;
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
    vec2 xi        = Hammersley(i, numSamples, a_RandBase);
    float phi      = xi.y * 2.0 * M_PI;
    float cosTheta = 1.0 - xi.x;
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    return vec3(cos(phi) * sinTheta, cosTheta, sin(phi) * sinTheta);
}

float ReduceLightBleeding(float p_max, float Amount)
{
    // Remove the [0, Amount] tail and linearly rescale (Amount, 1].
    return saturate(smoothstep(Amount, 1, p_max));
}

float ChebyshevUpperBound(vec2 Moments, float t)
{
    float g_MinVariance   = 0.0001f;
    float g_LightBleedRed = 0.9f;
    // One-tailed inequality valid if t > Moments.x
    float p = (t <= Moments.x) ? 1 : 0; // TODO check if step could replace that
    // Compute variance.
    float Variance = Moments.y - (Moments.x * Moments.x);
    Variance       = max(Variance, g_MinVariance);
    // Compute probabilistic upper bound.
    float d     = t - Moments.x;
    float p_max = ReduceLightBleeding(Variance / (Variance + d * d), g_LightBleedRed);
    return max(p, p_max);
}
float ShadowContribution(vec2 Moments, float DistanceToLight)
{
    // Compute the Chebyshev upper bound.
    return ChebyshevUpperBound(Moments, DistanceToLight);
}

float SampleShadowMap(
    IN(sampler2DArray) a_Sampler,
    IN(vec2) a_ShadowCoord,
    IN(int) a_ViewportIndex,
    IN(float) a_LightDistance)
{
    vec2 moments = texture(a_Sampler, vec3(a_ShadowCoord, a_ViewportIndex)).xy;
    return ShadowContribution(moments, a_LightDistance);
}

float SampleShadowMap(IN(sampler2DArray) a_Sampler, IN(ShadowDirData) a_Data)
{
    const vec4 viewPos     = a_Data.view * vec4(a_Data.surfacePosition, 1.0);
    const vec4 shadowPos   = a_Data.projection * viewPos;
    const vec2 shadowCoord = (shadowPos.xy / shadowPos.w) * 0.5 + 0.5;
    const int layerIndex   = 0;
    const float lightDist  = -(viewPos.z / viewPos.w);
    return SampleShadowMap(
        a_Sampler,
        shadowCoord.xy,
        layerIndex,
        lightDist);
}

float SampleShadowMap(IN(sampler2DArray) a_Sampler, IN(ShadowSpotData) a_Data)
{
    const vec4 viewPos     = a_Data.view * vec4(a_Data.surfacePosition, 1.0);
    const vec4 shadowPos   = a_Data.projection * viewPos;
    const vec2 shadowCoord = (shadowPos.xy / shadowPos.w) * 0.5 + 0.5;
    const int layerIndex   = 0;
    const float lightDist  = -(viewPos.z / viewPos.w);
    return SampleShadowMap(
        a_Sampler,
        shadowCoord,
        layerIndex,
        lightDist);
}

float SampleShadowMap(IN(sampler2DArray) a_Sampler, IN(ShadowPointData) a_Data)
{
    const vec3 sampleVec  = normalize(a_Data.surfacePosition - a_Data.lightPosition);
    const float lightDist = distance(a_Data.lightPosition, a_Data.surfacePosition);
    vec3 sampleUV         = CubemapSampleDirToUVW(normalize(sampleVec));
    vec2 moments          = texture(a_Sampler, sampleUV).xy;
    return ShadowContribution(moments, lightDist);
}

#endif //__cplusplus
#endif // SAMPLE_SHADOW_MAP_GLSL