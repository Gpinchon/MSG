//////////////////////////////////////// INCLUDES
#include <BRDF.glsl>
#include <Bindings.glsl>
#include <Camera.glsl>
#include <MaterialInputs.glsl>
#include <Random.glsl>
//////////////////////////////////////// INCLUDES

//////////////////////////////////////// STAGE INPUTS
layout(location = 0) in float in_Depth;
layout(location = 1) in float in_DepthRange;
layout(location = 4) in vec2 in_TexCoord[ATTRIB_TEXCOORD_COUNT];
//////////////////////////////////////// STAGE INPUTS

//////////////////////////////////////// STAGE OUTPUTS
//////////////////////////////////////// STAGE OUTPUTS

//////////////////////////////////////// UNIFORMS
// None
//////////////////////////////////////// UNIFORMS

//////////////////////////////////////// SSBOS
// None
//////////////////////////////////////// SSBOS
#define TRANSPARENCY_THRESHOLD 0.9

void main()
{
    const float randVal = Dither(ivec2(gl_FragCoord.xy + ivec2(in_Depth * in_DepthRange)));
    const BRDF brdf     = GetBRDF(SampleTexturesMaterialLod(in_TexCoord, 0), vec3(1));
    if (brdf.transparency < TRANSPARENCY_THRESHOLD && randVal > brdf.transparency)
        discard;
    gl_FragDepth = in_Depth;
}
