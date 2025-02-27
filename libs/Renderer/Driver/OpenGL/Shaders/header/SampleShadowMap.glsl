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
 * @brief samples the specified sampler2DShadow, bias formula taken from
 * Adaptive Depth Bias for Soft Shadows by Alexander Ehm, Alexander Ederer, Andreas Klein and Alfred Nischwitz
 *
 * @arg a_Near : the light's projection near plane
 * @arg a_Far : the light's projection far plane
 * @arg a_Projection : the shadowmap projection matrix
 * @arg a_WorldPosition : the current fragment's world position
 * @arg a_RandBase : the base used for shadow bluring, best value : ivec3(gl_FragCoord.xy, FrameNumber % 8)
 * @arg a_ShadowTexture : the shadow map
 * @see https://w3-o.cs.hm.edu/users/nischwit/public_html/AdaptiveDepthBias_WSCG.pdf
 */
float SampleShadowMap(
    IN(float) a_Near,
    IN(float) a_Far,
    IN(mat4) a_Projection,
    IN(vec3) a_WorldPosition,
    IN(ivec3) a_RandBase,
    sampler2DShadow a_ShadowTexture)
{
    const vec4 shadowPos     = a_Projection * vec4(a_WorldPosition, 1.0);
    const vec3 shadowCoord   = vec3(shadowPos.xyz / shadowPos.w) * 0.5 + 0.5;
    const float shadowDepth  = shadowCoord.z;
    const float shadowDepthR = a_Far - a_Near;
    const float shadowK      = 0.1f;
    const float shadowBias   = pow(a_Far - shadowDepth * shadowDepthR, 2.f) / (a_Far * a_Near * shadowDepthR) * shadowK;
    const uvec2 random       = Rand3DPCG16(a_RandBase).xy;
    float shadow             = 0;
    for (int i = 0; i < SHADOW_SAMPLES; i++) {
        vec2 sampleUV = shadowCoord.xy + Hammersley(i, SHADOW_SAMPLES, random) * SHADOWBLURRADIUS;
        shadow += texture(a_ShadowTexture, vec3(sampleUV, shadowCoord.z - shadowBias));
    }
    return (shadow / float(SHADOW_SAMPLES));
}

#endif //__cplusplus
#endif // SAMPLE_SHADOW_MAP_GLSL