#ifndef SAMPLE_SHADOW_MAP
#define SAMPLE_SHADOW_MAP

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

float SampleShadowMap(
    IN(float) bias,
    IN(mat4) projection,
    IN(vec3) worldPosition,
    IN(vec2) randBase,
    sampler2DShadow shadowTexture)
{
    const vec4 shadowPos = projection * vec4(worldPosition, 1.0);
    const vec3 projCoord = vec3(shadowPos.xyz / shadowPos.w) * 0.5 + 0.5;
#ifdef SHADOWBLURRADIUS
    const float sampleOffset = SHADOWBLURRADIUS;
#else
    const float sampleOffset = 5.f / 256.f;
#endif
    float shadow       = 0;
    const uvec2 Random = Rand3DPCG16(ivec3(randBase, FrameNumber % 8)).xy;
    for (int i = 0; i < SHADOW_SAMPLES; i++) {
        vec2 sampleUV = projCoord.xy + Hammersley(i, SHADOW_SAMPLES, Random) * sampleOffset;
        shadow += texture(shadowTexture, vec3(sampleUV, projCoord.z - bias));
    }
    return (shadow / float(SHADOW_SAMPLES));
}

#endif // SAMPLE_SHADOW_MAP
