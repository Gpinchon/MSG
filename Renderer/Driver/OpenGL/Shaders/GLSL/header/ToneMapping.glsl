#ifndef TONEMAPPING_GLSL
#define TONEMAPPING_GLSL

#include <Functions.glsl>

#define GAMMA     (2.2)
#define INV_GAMMA (1.0 / GAMMA)

vec3 SRGBToLinear(IN(vec3) a_Color)
{
    return pow(a_Color, vec3(GAMMA));
}

vec3 LinearToSRGB(IN(vec3) a_Color)
{
    return pow(a_Color, vec3(INV_GAMMA));
}

vec3 ReinhardTonemapping(IN(vec3) a_Color)
{
    // reinhard tone mapping
    const vec3 mapped = a_Color / (a_Color + 1.f);
    // gamma correction
    return LinearToSRGB(mapped);
}

#endif // TONEMAPPING_GLSL