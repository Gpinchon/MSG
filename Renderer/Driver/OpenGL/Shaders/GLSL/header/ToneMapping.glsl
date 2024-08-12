#ifndef TONEMAPPING_GLSL
#define TONEMAPPING_GLSL

#include <Functions.glsl>

#define DEFAULT_GAMMA (2.2)

vec3 SRGBToLinear(IN(vec3) a_Color, IN(float) a_Gamma)
{
    return pow(a_Color, vec3(a_Gamma));
}

vec3 SRGBToLinear(IN(vec3) a_Color)
{
    return SRGBToLinear(a_Color, DEFAULT_GAMMA);
}

vec3 LinearToSRGB(IN(vec3) a_Color, IN(float) a_Gamma)
{
    return pow(a_Color, vec3(1.f / a_Gamma));
}

vec3 LinearToSRGB(IN(vec3) a_Color)
{
    return LinearToSRGB(a_Color, DEFAULT_GAMMA);
}

vec3 ReinhardTonemapping(IN(vec3) a_Color, IN(float) a_Gamma)
{
    // reinhard tone mapping
    const vec3 mapped = a_Color / (a_Color + 1.f);
    // gamma correction
    return LinearToSRGB(mapped, a_Gamma);
}

vec3 ReinhardTonemapping(IN(vec3) a_Color)
{
    return ReinhardTonemapping(a_Color, DEFAULT_GAMMA);
}

vec3 CZMSaturation(vec3 a_Color, float a_Adjustment)
{
    // Algorithm from Chapter 16 of OpenGL Shading Language
    const vec3 W   = vec3(0.2125, 0.7154, 0.0721);
    vec3 intensity = vec3(dot(a_Color, W));
    return mix(intensity, a_Color, a_Adjustment);
}

#endif // TONEMAPPING_GLSL