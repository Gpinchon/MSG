//////////////////////////////////////// INCLUDES
#include <BRDF.glsl>
#include <Bindings.glsl>
#include <Camera.glsl>
#include <MaterialInputs.glsl>
#include <Random.glsl>
//////////////////////////////////////// INCLUDES

//////////////////////////////////////// STAGE INPUTS
layout(location = 0) in float in_Depth;
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
void main()
{
    // const float randVal = BlueNoise(gl_FragCoord.xy * in_Depth);
    const float randVal = Dither(ivec2(gl_FragCoord.xy));
    const BRDF brdf     = GetBRDF(SampleTexturesMaterial(in_TexCoord), vec3(1));
    if (brdf.transparency <= 0.9 && randVal > brdf.transparency)
        discard;
    gl_FragDepth = in_Depth;
}
