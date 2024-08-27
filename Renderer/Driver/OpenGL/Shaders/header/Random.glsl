#ifndef RANDOM_GLSL
#define RANDOM_GLSL

#include <Functions.glsl>
#include <Types.glsl>

#ifdef __cplusplus
namespace TabGraph::Renderer::GLSL {
#endif //__cplusplus

float InterleavedGradientNoise(vec2 uv, float FrameId)
{
    // magic values are found by experimentation
    uv += FrameId * (vec2(47, 17) * 0.695f);

    const vec3 magic = vec3(0.06711056f, 0.00583715f, 52.9829189f);
    return fract(magic.z * fract(dot(uv, magic.xy)));
}

vec2 Hammersley16(IN(uint) Index, IN(uint) NumSamples, IN(uvec2) Random)
{
    float E1 = fract(Index / float(NumSamples) + float(Random.x) * (1.0 / 65536.0));
    float E2 = float((bitfieldReverse(Index) >> 16) ^ Random.y) * (1.0 / 65536.0);
    return vec2(E1, E2);
}

vec2 Hammersley(uint Index, uint NumSamples, uvec2 Random)
{
    float E1 = fract(Index / float(NumSamples) + float(Random.x & 0xffff) / (1 << 16));
    float E2 = float(bitfieldReverse(Index) ^ Random.y) * 2.3283064365386963e-10;
    return vec2(E1, E2);
}

uvec3 Rand3DPCG16(ivec3 p)
{
    uvec3 v = uvec3(p);
    v       = v * 1664525u + 1013904223u;
    v.x += v.y * v.z;
    v.y += v.z * v.x;
    v.z += v.x * v.y;
    v.x += v.y * v.z;
    v.y += v.z * v.x;
    v.z += v.x * v.y;
    return v >> 16u;
}

#ifdef __cplusplus
}
#endif //__cplusplus
#endif // RANDOM_GLSL
