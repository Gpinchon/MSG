//////////////////////////////////////// SHADER LAYOUT
layout(early_fragment_tests) in;
//////////////////////////////////////// SHADER LAYOUT

//////////////////////////////////////// INCLUDES
#include <BRDFInputs.glsl>
#include <Bindings.glsl>
#include <Functions.glsl>
#include <MaterialInputs.glsl>
//////////////////////////////////////// INCLUDES

//////////////////////////////////////// STAGE INPUTS
layout(location = 4) in vec2 in_TexCoord[ATTRIB_TEXCOORD_COUNT];
layout(location = 4 + ATTRIB_TEXCOORD_COUNT) in vec3 in_Color;
layout(location = 4 + ATTRIB_TEXCOORD_COUNT + 2) noperspective in float in_ViewDist;
//////////////////////////////////////// STAGE INPUTS

//////////////////////////////////////// STAGE OUTPUTS
// none, only depth
//////////////////////////////////////// STAGE OUTPUTS

//////////////////////////////////////// UNIFORMS
layout(binding = IMG_WBOIT_DEPTH, r32ui) coherent uniform uimage2D img_Depth;
//////////////////////////////////////// UNIFORMS

void main()
{
    const vec4 textureSamplesMaterials[] = SampleTexturesMaterial(in_TexCoord);
    const BRDF brdf                      = GetBRDF(textureSamplesMaterials, in_Color);
    if (brdf.transparency <= 0.003)
        discard;
    imageAtomicMin(img_Depth, ivec2(gl_FragCoord.xy), floatBitsToUint(in_ViewDist));
}