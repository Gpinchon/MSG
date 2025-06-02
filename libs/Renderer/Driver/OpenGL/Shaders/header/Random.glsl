#ifndef RANDOM_GLSL
#define RANDOM_GLSL

#include <Functions.glsl>
#include <Types.glsl>

#ifdef __cplusplus
namespace MSG::Renderer::GLSL {
#endif //__cplusplus

float Dither(IN(ivec2) a_Coord)
{
    const float ditherMat[8][8] = {
        { 0.0f / 64.f, 32.f / 64.f, 8.0f / 64.f, 40.f / 64.f, 2.0f / 64.f, 34.f / 64.f, 10.f / 64.f, 42.f / 64.f },
        { 48.f / 64.f, 16.f / 64.f, 56.f / 64.f, 24.f / 64.f, 50.f / 64.f, 18.f / 64.f, 58.f / 64.f, 26.f / 64.f },
        { 12.f / 64.f, 44.f / 64.f, 4.0f / 64.f, 36.f / 64.f, 14.f / 64.f, 46.f / 64.f, 6.0f / 64.f, 38.f / 64.f },
        { 60.f / 64.f, 28.f / 64.f, 52.f / 64.f, 20.f / 64.f, 62.f / 64.f, 30.f / 64.f, 54.f / 64.f, 22.f / 64.f },
        { 3.0f / 64.f, 35.f / 64.f, 11.f / 64.f, 43.f / 64.f, 1.0f / 64.f, 33.f / 64.f, 9.0f / 64.f, 41.f / 64.f },
        { 51.f / 64.f, 19.f / 64.f, 59.f / 64.f, 27.f / 64.f, 49.f / 64.f, 17.f / 64.f, 57.f / 64.f, 25.f / 64.f },
        { 15.f / 64.f, 47.f / 64.f, 7.0f / 64.f, 39.f / 64.f, 13.f / 64.f, 45.f / 64.f, 5.0f / 64.f, 37.f / 64.f },
        { 63.f / 64.f, 31.f / 64.f, 55.f / 64.f, 23.f / 64.f, 61.f / 64.f, 29.f / 64.f, 53.f / 64.f, 21.f / 64.f }
    };
    ivec2 coord = a_Coord % 8;
    return ditherMat[coord.x][coord.y];
}

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

/** @return 3 random values between 0 and uint16 max value */
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

/** @return 3 random values between 0 and uint32 max value */
uvec3 Rand3DPCG32(ivec3 p)
{
    // taking a signed int then reinterpreting as unsigned gives good behavior for negatives
    uvec3 v = uvec3(p);

    // Linear congruential step.
    v = v * 1664525u + 1013904223u;

    // swapping low and high bits makes all 32 bits pretty good
    v = v * (1u << 16u) + (v >> 16u);

    // final shuffle
    v.x += v.y * v.z;
    v.y += v.z * v.x;
    v.z += v.x * v.y;
    v.x += v.y * v.z;
    v.y += v.z * v.x;
    v.z += v.x * v.y;

    return v;
}

vec3 Halton235(uint a_Index)
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
