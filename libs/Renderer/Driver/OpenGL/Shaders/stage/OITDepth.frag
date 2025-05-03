//////////////////////////////////////// SHADER LAYOUT
layout(early_fragment_tests) in;
//////////////////////////////////////// SHADER LAYOUT

//////////////////////////////////////// INCLUDES
#include <BRDF.glsl>
#include <Bindings.glsl>
#include <Camera.glsl>
#include <MaterialInputs.glsl>
#include <OIT.glsl>
//////////////////////////////////////// INCLUDES

//////////////////////////////////////// STAGE INPUTS
layout(location = 4) in vec2 in_TexCoord[ATTRIB_TEXCOORD_COUNT];
//////////////////////////////////////// STAGE INPUTS

//////////////////////////////////////// STAGE OUTPUTS
// None
//////////////////////////////////////// STAGE OUTPUTS

//////////////////////////////////////// UNIFORMS
layout(binding = IMG_OIT_DEPTH, r32ui) uniform coherent uimage3D img_Depth;
//////////////////////////////////////// UNIFORMS

//////////////////////////////////////// SSBOS
// None
//////////////////////////////////////// SSBOS

void main()
{
    if (GetBRDF(SampleTexturesMaterial(in_TexCoord), vec3(0)).transparency <= 0.003)
        discard; // early discard because current fragment is transparent
    int layer = 0;
    uint zCur = floatBitsToUint(gl_FragCoord.z);
    /**
     * EARLY Z TEST
     * check if we can avoid this shader altogether
     * or start iterating from the beginning of the later half
     */
    {
        uint zTest = imageLoad(img_Depth, ivec3(gl_FragCoord.xy, OIT_LAYERS - 1))[0];
        if (zCur > zTest) // current fragment behind farthest depth
            return;
        zTest = imageLoad(img_Depth, ivec3(gl_FragCoord.xy, OIT_LAYERS / 2))[0];
        if (zCur > zTest) // current fragment can be stored in the later half of the depth buffer
            layer = (OIT_LAYERS / 2);
    }
    for (; layer < OIT_LAYERS; layer++) {
        const uint zTest = imageAtomicMin(img_Depth, ivec3(gl_FragCoord.xy, layer), zCur);
        if (zTest == 0xFFFFFFFFu || zTest == zCur)
            break; // we inserted or found a fragment with the same depth
        zCur = max(zTest, zCur); // now push the remaining fragments back
    }
}