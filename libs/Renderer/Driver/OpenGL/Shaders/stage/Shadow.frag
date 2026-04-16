// #if MATERIAL_ALPHA_MODE == MATERIAL_ALPHA_MODE_OPAQUE
// layout(early_fragment_tests) in;
// #endif
//////////////////////////////////////// INCLUDES
#include <BRDF.glsl>
#include <Bindings.glsl>
#include <MaterialInputs.glsl>
#include <Random.glsl>
//////////////////////////////////////// INCLUDES

//////////////////////////////////////// STAGE INPUTS
layout(location = 0) in float in_DepthRange;
layout(location = 1) in float in_Depth;
layout(location = 2) in float in_UnclampedDepth;
layout(location = 3) in vec2 in_TexCoord[ATTRIB_TEXCOORD_COUNT];
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
#if MATERIAL_ALPHA_MODE != MATERIAL_ALPHA_MODE_OPAQUE
    const float transparency = GetTransparency(SampleCDiffMaterial(in_TexCoord));
    if (transparency < TRANSPARENCY_THRESHOLD && randVal > transparency)
        discard;
#endif
    gl_FragDepth       = in_Depth; // required by AMD for some reason
    out_UnclampedDepth = vec4(in_UnclampedDepth);
}
