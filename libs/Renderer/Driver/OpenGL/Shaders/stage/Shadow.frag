//////////////////////////////////////// INCLUDES
#include <BRDF.glsl>
#include <Bindings.glsl>
#include <Camera.glsl>
#include <MaterialInputs.glsl>
//////////////////////////////////////// INCLUDES

//////////////////////////////////////// STAGE INPUTS
#if SHADOW_CUBE
layout(location = 0) in float in_Depth;
#endif // SHADOW_CUBE
layout(location = 4) in vec2 in_TexCoord[ATTRIB_TEXCOORD_COUNT];
//////////////////////////////////////// STAGE INPUTS

//////////////////////////////////////// STAGE OUTPUTS
// None, only output depth
//////////////////////////////////////// STAGE OUTPUTS

//////////////////////////////////////// UNIFORMS
// None
//////////////////////////////////////// UNIFORMS

//////////////////////////////////////// SSBOS
// None
//////////////////////////////////////// SSBOS

float ditherMat[8][8] = {
    { 0.0f / 64.f, 32.f / 64.f, 8.0f / 64.f, 40.f / 64.f, 2.0f / 64.f, 34.f / 64.f, 10.f / 64.f, 42.f / 64.f },
    { 48.f / 64.f, 16.f / 64.f, 56.f / 64.f, 24.f / 64.f, 50.f / 64.f, 18.f / 64.f, 58.f / 64.f, 26.f / 64.f },
    { 12.f / 64.f, 44.f / 64.f, 4.0f / 64.f, 36.f / 64.f, 14.f / 64.f, 46.f / 64.f, 6.0f / 64.f, 38.f / 64.f },
    { 60.f / 64.f, 28.f / 64.f, 52.f / 64.f, 20.f / 64.f, 62.f / 64.f, 30.f / 64.f, 54.f / 64.f, 22.f / 64.f },
    { 3.0f / 64.f, 35.f / 64.f, 11.f / 64.f, 43.f / 64.f, 1.0f / 64.f, 33.f / 64.f, 9.0f / 64.f, 41.f / 64.f },
    { 51.f / 64.f, 19.f / 64.f, 59.f / 64.f, 27.f / 64.f, 49.f / 64.f, 17.f / 64.f, 57.f / 64.f, 25.f / 64.f },
    { 15.f / 64.f, 47.f / 64.f, 7.0f / 64.f, 39.f / 64.f, 13.f / 64.f, 45.f / 64.f, 5.0f / 64.f, 37.f / 64.f },
    { 63.f / 64.f, 31.f / 64.f, 55.f / 64.f, 23.f / 64.f, 61.f / 64.f, 29.f / 64.f, 53.f / 64.f, 21.f / 64.f }
};

float Dither(vec2 a_Coord, float a_Threshold)
{
    const ivec2 coords = ivec2(mod(a_Coord, 8.f));
    return ditherMat[coords[0]][coords[1]] >= a_Threshold ? 1.f : 0.f;
}

void main()
{
    const vec4 textureSamplesMaterials[] = SampleTexturesMaterial(in_TexCoord);
    const BRDF brdf                      = GetBRDF(textureSamplesMaterials, vec3(0));
    if (Dither(gl_FragCoord.xy, 1 - brdf.transparency) == 0)
        discard;
#if SHADOW_CUBE
    gl_FragDepth = in_Depth;
#endif // SHADOW_CUBE
}