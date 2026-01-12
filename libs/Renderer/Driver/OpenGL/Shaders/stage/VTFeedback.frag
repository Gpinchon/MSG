//////////////////////////////////////// SHADER LAYOUT
layout(early_fragment_tests) in;
//////////////////////////////////////// SHADER LAYOUT

//////////////////////////////////////// INCLUDES
#include <Bindings.glsl>
#include <Functions.glsl>
#include <VirtualTexturing.glsl>
//////////////////////////////////////// INCLUDES

//////////////////////////////////////// STAGE INPUTS
layout(location = 0) in vec3 in_WorldPosition;
layout(location = 1) in vec3 in_WorldNormal;
layout(location = 2) in vec3 in_WorldTangent;
layout(location = 3) in vec3 in_WorldBitangent;
layout(location = 4) in vec2 in_TexCoord[ATTRIB_TEXCOORD_COUNT];
layout(location = 4 + ATTRIB_TEXCOORD_COUNT) in vec3 in_Color;
layout(location = 4 + ATTRIB_TEXCOORD_COUNT + 1) noperspective in vec3 in_NDCPosition;
layout(location = 4 + ATTRIB_TEXCOORD_COUNT + 2) in vec4 in_Position;
layout(location = 4 + ATTRIB_TEXCOORD_COUNT + 3) in vec4 in_Position_Previous;
//////////////////////////////////////// STAGE INPUTS

//////////////////////////////////////// STAGE OUTPUTS
// None
//////////////////////////////////////// STAGE OUTPUTS

//////////////////////////////////////// UNIFORMS
// None
//////////////////////////////////////// UNIFORMS

//////////////////////////////////////// SSBOS
layout(binding = 0) restrict readonly buffer VTMaterialBlock
{
    VTMaterialInfo ssbo_MaterialInfo;
};

layout(std430, binding = 1) restrict coherent buffer VTOutputBlock
{
    VTFeedbackOutput ssbo_Output[];
};
//////////////////////////////////////// SSBOS

float Mirror(IN(float) a_Val) { return a_Val >= 0.f ? a_Val : -(1.f + a_Val); }

float WrapTexelCoord(IN(uint) a_Wrap, IN(float) a_Size, IN(float) a_Coord)
{
    if (a_Wrap == VT_WRAP_REPEAT)
        return mod(mod(a_Coord, a_Size) + a_Size, a_Size); // handle negative indice as well
    else if (a_Wrap == VT_WRAP_CLAMP)
        return clamp(a_Coord, 0.f, a_Size - 1);
    else if (a_Wrap == VT_WRAP_REPEAT_MIRROR)
        return (a_Size - 1) - Mirror(mod(a_Coord, (2 * a_Size))) - a_Size;
    else if (a_Wrap == VT_WRAP_CLAMP_MIRROR)
        return clamp(Mirror(a_Coord), 0.f, a_Size - 1);
    else
        return a_Coord;
}

vec2 WrapTexelCoords(
    IN(uint) a_WrapS,
    IN(uint) a_WrapT,
    IN(vec2) a_TextureSize,
    IN(vec2) a_TexelCoord)
{
    return vec2(
        WrapTexelCoord(a_WrapS, a_TextureSize[0], a_TexelCoord[0]),
        WrapTexelCoord(a_WrapT, a_TextureSize[1], a_TexelCoord[1]));
}

void main()
{
    VTFeedbackOutput feedbackOutput[SAMPLERS_MATERIAL_COUNT];
    for (uint i = 0; i < SAMPLERS_MATERIAL_COUNT; ++i) {
        const VTTextureInfo texInfo = ssbo_MaterialInfo.textures[i];
        if (texInfo.id == 0) // no texture there
            continue;
        const vec2 texCoord  = in_TexCoord[texInfo.texCoord];
        const vec2 scale     = texInfo.transform.scale;
        const vec2 offset    = texInfo.transform.offset;
        const float rotation = texInfo.transform.rotation;
        mat3 rotationMat     = mat3(
            cos(rotation), sin(rotation), 0,
            -sin(rotation), cos(rotation), 0,
            0, 0, 1);
        vec2 transformedUV = (rotationMat * vec3(texCoord.xy, 1)).xy * scale + offset;
        vec2 transformedTC = transformedUV * texInfo.texSize;
        vec2 wrappedTC     = WrapTexelCoords(
            texInfo.wrapS, texInfo.wrapT,
            texInfo.texSize,
            transformedTC);
        vec2 uvMin               = floor(wrappedTC) / texInfo.texSize;
        vec2 uvMax               = ceil(wrappedTC) / texInfo.texSize;
        float lod                = VTComputeLOD(transformedUV, texInfo.maxAniso) + texInfo.lodBias;
        float lodMin             = floor(lod);
        float lodMax             = ceil(lod);
        feedbackOutput[i].minUV  = uvMin;
        feedbackOutput[i].maxUV  = uvMax;
        feedbackOutput[i].minMip = lodMin;
        feedbackOutput[i].maxMip = lodMax;
    }
    memoryBarrierBuffer();
    for (uint i = 0; i < SAMPLERS_MATERIAL_COUNT; ++i) {
        const VTTextureInfo texInfo    = ssbo_MaterialInfo.textures[i];
        ssbo_Output[texInfo.id].minUV  = min(ssbo_Output[texInfo.id].minUV, feedbackOutput[i].minUV);
        ssbo_Output[texInfo.id].maxUV  = max(ssbo_Output[texInfo.id].maxUV, feedbackOutput[i].maxUV);
        ssbo_Output[texInfo.id].minMip = min(ssbo_Output[texInfo.id].minMip, feedbackOutput[i].minMip);
        ssbo_Output[texInfo.id].maxMip = max(ssbo_Output[texInfo.id].maxMip, feedbackOutput[i].maxMip);
    }
}