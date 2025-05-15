#ifndef RANDOM_GLSL
#define RANDOM_GLSL

#include <Functions.glsl>
#include <Types.glsl>

#ifdef __cplusplus
namespace MSG::Renderer::GLSL {
#endif //__cplusplus

/** @see https://www.shadertoy.com/view/tllcR2 */
#define T(ch)   texelFetch(ch, ivec2(U) % textureSize(ch, 0), 0).x
#define hash(p) fract(sin(dot(p, vec2(11.9898, 78.233))) * 43758.5453) // iq suggestion, for Windows

float BlueNoise(vec2 U)
{ // 5-tap version
    float v = hash(U + vec2(-1, 0))
        + hash(U + vec2(1, 0))
        + hash(U + vec2(0, 1))
        + hash(U + vec2(0, -1));
    return hash(U) - v / 4. + .5;
}

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

vec2 Hammersley(IN(uint) Index, IN(uint) NumSamples, IN(uvec2) Random)
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

vec3 Halton235(int a_Index)
{
    const vec3 halton235LUT[16] = vec3[16](
        vec3(0.50000, 0.333333, 0.20),
        vec3(0.25000, 0.666667, 0.40),
        vec3(0.75000, 0.111111, 0.60),
        vec3(0.12500, 0.444444, 0.60),
        vec3(0.62500, 0.777778, 0.04),
        vec3(0.37500, 0.222222, 0.24),
        vec3(0.87500, 0.555556, 0.44),
        vec3(0.06250, 0.888889, 0.64),
        vec3(0.56250, 0.037037, 0.84),
        vec3(0.31250, 0.370370, 0.08),
        vec3(0.81250, 0.703704, 0.28),
        vec3(0.18750, 0.148148, 0.48),
        vec3(0.68750, 0.481482, 0.68),
        vec3(0.43750, 0.814815, 0.88),
        vec3(0.93750, 0.259259, 0.12),
        vec3(0.03125, 0.592593, 0.32));
    return halton235LUT[a_Index % 16];
}

#ifdef __cplusplus
}
#endif //__cplusplus
#endif // RANDOM_GLSL
