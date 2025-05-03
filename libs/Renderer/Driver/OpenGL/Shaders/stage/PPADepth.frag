//////////////////////////////////////// SHADER LAYOUT
layout(early_fragment_tests) in;
//////////////////////////////////////// SHADER LAYOUT

//////////////////////////////////////// INCLUDES
#include <BRDF.glsl>
#include <Bindings.glsl>
#include <Camera.glsl>
#include <MaterialInputs.glsl>
#include <PPA.glsl>
//////////////////////////////////////// INCLUDES

//////////////////////////////////////// STAGE INPUTS
layout(location = 4) in vec2 in_TexCoord[ATTRIB_TEXCOORD_COUNT];
//////////////////////////////////////// STAGE INPUTS

//////////////////////////////////////// STAGE OUTPUTS
// None
//////////////////////////////////////// STAGE OUTPUTS

//////////////////////////////////////// UNIFORMS
layout(binding = IMG_PPA_DEPTH, r32ui) uniform coherent uimage3D img_Depth;
//////////////////////////////////////// UNIFORMS

//////////////////////////////////////// SSBOS
// None
//////////////////////////////////////// SSBOS

void main()
{
    const vec4 textureSamplesMaterials[] = SampleTexturesMaterial(in_TexCoord);
    const BRDF brdf                      = GetBRDF(textureSamplesMaterials, vec3(0));
    if (brdf.transparency <= 0.003)
        discard;
    int i     = 0;
    uint zCur = floatBitsToUint(gl_FragCoord.z);
    // EARLY Z TEST
    {
        uint zTest = imageLoad(img_Depth, ivec3(gl_FragCoord.xy, PPA_LAYERS - 1))[0];
        if (zCur > zTest)
            return; // current fragment behind farthest depth
        zTest = imageLoad(img_Depth, ivec3(gl_FragCoord.xy, PPA_LAYERS / 2))[0];
        if (zCur > zTest)
            i = (PPA_LAYERS / 2); // current fragment can be stored in the later half of the depth buffer
    }
    for (; i < PPA_LAYERS; i++) {
        const uint zTest = imageAtomicMin(img_Depth, ivec3(gl_FragCoord.xy, i), zCur);
        if (zTest == 0xFFFFFFFFu || zTest == zCur)
            break;
        zCur = max(zTest, zCur);
    }
}