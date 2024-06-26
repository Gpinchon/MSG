#ifndef SAMPLE_SHADOW_MAP_GLSL
#define SAMPLE_SHADOW_MAP_GLSL

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
 * @arg a_Bias : the bias used to try and avoid shadow acnee
 * @arg a_Projection : the shadowmap projection matrix
 * @arg a_WorldPosition : the current fragment's world position
 * @arg a_RandBase : the base used for shadow bluring, best value : ivec3(gl_FragCoord.xy, FrameNumber % 8)
 * @arg a_ShadowTexture : the shadow map
 */
float SampleShadowMap(
    IN(float) a_Bias,
    IN(mat4) a_Projection,
    IN(vec3) a_WorldPosition,
    IN(ivec3) a_RandBase,
    sampler2DShadow a_ShadowTexture)
{
    const vec4 shadowPos = a_Projection * vec4(a_WorldPosition, 1.0);
    const vec3 projCoord = vec3(shadowPos.xyz / shadowPos.w) * 0.5 + 0.5;
    const uvec2 random   = Rand3DPCG16(a_RandBase).xy;
    float shadow         = 0;
    for (int i = 0; i < SHADOW_SAMPLES; i++) {
        vec2 sampleUV = projCoord.xy + Hammersley(i, SHADOW_SAMPLES, random) * SHADOWBLURRADIUS;
        shadow += texture(a_ShadowTexture, vec3(sampleUV, projCoord.z - a_Bias));
    }
    return (shadow / float(SHADOW_SAMPLES));
}

#endif // SAMPLE_SHADOW_MAP_GLSL
