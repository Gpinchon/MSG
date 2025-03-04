#ifndef SAMPLE_SHADOW_MAP_GLSL
#define SAMPLE_SHADOW_MAP_GLSL
#ifndef __cplusplus

#include <Random.glsl>
#include <Types.glsl>

#ifndef SHADOWBLURRADIUS
#define SHADOWBLURRADIUS 5.f / 256.f
#endif // SHADOWBLURRADIUS

#if SHADOW_QUALITY == 1
#define SHADOW_SAMPLES 1
#elif SHADOW_QUALITY == 2
#define SHADOW_SAMPLES 3
#elif SHADOW_QUALITY == 3
#define SHADOW_SAMPLES 5
#else // SHADOW_QUALITY == 4
#define SHADOW_SAMPLES 9
#endif

/**
 * @brief Adaptive Depth Bias for Soft Shadows by Alexander Ehm, Alexander Ederer, Andreas Klein and Alfred Nischwitz
 *
 * @see https://w3-o.cs.hm.edu/users/nischwit/public_html/AdaptiveDepthBias_WSCG.pdf
 */
float ShadowBias(IN(float) a_Depth, IN(float) a_Near, IN(float) a_Far)
{
    const float K          = 0.001f;
    const float depthRange = a_Far - a_Near;
    return pow(a_Far - a_Depth * depthRange, 2.f) / (a_Far * a_Near * depthRange) * K;
}

float SampleShadowMap(
    IN(float) a_Bias,
    IN(vec3) a_ShadowCoord,
    IN(ivec3) a_RandBase,
    sampler2DShadow a_ShadowTexture)
{
    const uvec2 random = Rand3DPCG16(a_RandBase).xy;
    float shadow       = 0;
    for (int i = 0; i < SHADOW_SAMPLES; i++) {
        vec2 sampleUV = a_ShadowCoord.xy + Hammersley(i, SHADOW_SAMPLES, random) * SHADOWBLURRADIUS;
        shadow += texture(a_ShadowTexture, vec3(sampleUV, a_ShadowCoord.z - a_Bias));
    }
    return (shadow / float(SHADOW_SAMPLES));
}

float SampleShadowMap(
    IN(float) a_Bias,
    IN(mat4) a_Projection,
    IN(vec3) a_WorldPosition,
    IN(ivec3) a_RandBase,
    sampler2DShadow a_ShadowTexture)
{
    const vec4 shadowPos   = a_Projection * vec4(a_WorldPosition, 1.0);
    const vec3 shadowCoord = vec3(shadowPos.xyz / shadowPos.w) * 0.5 + 0.5;
    return SampleShadowMap(a_Bias, shadowCoord, a_RandBase, a_ShadowTexture);
}

/**
 * @brief samples the specified sampler2DShadow
 *
 * @arg a_Near : the light's projection near plane
 * @arg a_Far : the light's projection far plane
 * @arg a_Projection : the shadowmap projection matrix
 * @arg a_WorldPosition : the current fragment's world position
 * @arg a_RandBase : the base used for shadow bluring, best value : ivec3(gl_FragCoord.xy, FrameNumber % 8)
 * @arg a_ShadowTexture : the shadow map
 */
float SampleShadowMap(
    IN(float) a_Near,
    IN(float) a_Far,
    IN(mat4) a_Projection,
    IN(vec3) a_WorldPosition,
    IN(ivec3) a_RandBase,
    sampler2DShadow a_ShadowTexture)
{
    const vec4 shadowPos   = a_Projection * vec4(a_WorldPosition, 1.0);
    const vec3 shadowCoord = vec3(shadowPos.xyz / shadowPos.w) * 0.5 + 0.5;
    const float shadowBias = ShadowBias(shadowCoord.z, a_Near, a_Far);
    return SampleShadowMap(shadowBias, shadowCoord, a_RandBase, a_ShadowTexture);
}

vec3 SampleHemisphere_Uniform(IN(uint) i, IN(uint) numSamples, IN(uvec2) a_RandBase)
{
    // Returns a 3D sample vector orientated around (0.0, 1.0, 0.0)
    // For practical use, must rotate with a rotation matrix (or whatever
    // your preferred approach is) for use with normals, etc.

    vec2 xi = Hammersley(i, numSamples, a_RandBase);

    float phi      = xi.y * 2.0 * M_PI;
    float cosTheta = 1.0 - xi.x;
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    return vec3(cos(phi) * sinTheta, cosTheta, sin(phi) * sinTheta);
}

/**
 * @brief samples a cube shadow map
 *
 * @param a_Near the light's near distance
 * @param a_Far the light's far distance
 * @param a_LightPosition the light's world position
 * @param a_WorldPosition the surface's world position
 * @param a_RandBase the base used for shadow bluring, best value : ivec3(gl_FragCoord.xy, FrameNumber % 8)
 * @param a_ShadowTexture the shadow map to sample
 */
float SampleShadowMap(
    IN(float) a_Near,
    IN(float) a_Far,
    IN(vec3) a_LightPosition,
    IN(vec3) a_WorldPosition,
    IN(ivec3) a_RandBase,
    samplerCubeShadow a_ShadowTexture)
{
    vec3 shadowVec         = a_WorldPosition - a_LightPosition;
    const float dist       = length(shadowVec);
    const float depth      = remap(dist, a_Near, a_Far, 0, 1);
    const float shadowBias = ShadowBias(depth, a_Near, a_Far);
    const uvec2 random     = Rand3DPCG16(a_RandBase).xy;
    shadowVec              = normalize(shadowVec);
    float shadow           = 0;
    for (int i = 0; i < SHADOW_SAMPLES; i++) {
        vec3 sampleVec = shadowVec + SampleHemisphere_Uniform(i, SHADOW_SAMPLES, random) * SHADOWBLURRADIUS;
        shadow += texture(a_ShadowTexture, vec4(sampleVec, depth - shadowBias));
    }
    return shadow / float(SHADOW_SAMPLES);
}

#endif //__cplusplus
#endif // SAMPLE_SHADOW_MAP_GLSL