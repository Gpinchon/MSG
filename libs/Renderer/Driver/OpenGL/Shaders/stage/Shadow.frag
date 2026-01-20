//////////////////////////////////////// INCLUDES
#include <BRDF.glsl>
#include <Bindings.glsl>
#include <MaterialInputs.glsl>
#include <Random.glsl>
//////////////////////////////////////// INCLUDES

//////////////////////////////////////// STAGE INPUTS
layout(location = 0) in float in_DepthRange;
layout(location = 1) in float in_UnclampedDepth;
layout(location = 2) in vec2 in_TexCoord[ATTRIB_TEXCOORD_COUNT];
//////////////////////////////////////// STAGE INPUTS

//////////////////////////////////////// STAGE OUTPUTS
layout(location = 0) out vec4 out_UnclampedDepth;
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
    const float randVal = Dither(ivec2(gl_FragCoord.xy + vec2(in_UnclampedDepth / in_DepthRange * 100.f)));
    const BRDF brdf     = GetBRDF(SampleTexturesMaterialLod(in_TexCoord, 0), vec3(1));
    if (brdf.transparency < TRANSPARENCY_THRESHOLD && randVal > brdf.transparency)
        discard;
    out_UnclampedDepth = vec4(in_UnclampedDepth);
}
