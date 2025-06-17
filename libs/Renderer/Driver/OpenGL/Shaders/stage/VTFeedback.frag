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

layout(std430, binding = 1) restrict buffer VTOutputBlock
{
    VTFeedbackOutput ssbo_Output[];
};
//////////////////////////////////////// SSBOS

int Mirror(IN(int) a_Val) { return a_Val >= 0 ? a_Val : -(1 + a_Val); }

int WrapTexelCoord(IN(uint) a_Wrap, IN(uint) a_TextureSize, IN(int) a_Coord)
{
    int coord = a_Coord;
    int size  = int(a_TextureSize);
    if (a_Wrap == VT_WRAP_REPEAT)
        return ((coord % size) + size) % size; // handle negative indice as well
    else if (a_Wrap == VT_WRAP_CLAMP)
        return clamp(coord, 0, size - 1);
    else if (a_Wrap == VT_WRAP_REPEAT_MIRROR)
        return (size - 1) - Mirror((coord % (2 * size))) - size;
    else if (a_Wrap == VT_WRAP_CLAMP_MIRROR)
        return clamp(Mirror(coord), 0, size - 1);
    else
        return a_Coord;
}

ivec2 WrapTexelCoords(
    IN(uint) a_WrapS,
    IN(uint) a_WrapT,
    IN(uvec2) a_TextureSize,
    IN(ivec2) a_TexelCoord)
{
    return ivec2(
        WrapTexelCoord(a_WrapS, a_TextureSize[0], a_TexelCoord[0]),
        WrapTexelCoord(a_WrapT, a_TextureSize[1], a_TexelCoord[1]));
}

/**
 * @brief This computation should correspond to GL 2.0 specs equation 3.18
 * https://registry.khronos.org/OpenGL/specs/gl/glspec20.pdf
 *
 * @return the unclamped desired LOD
 */
float ComputeLOD(IN(float) a_MaxAniso, IN(ivec2) a_TexCoord)
{
    float maxAnisoLog2 = log2(a_MaxAniso);
    vec2 dx            = dFdx(a_TexCoord);
    vec2 dy            = dFdy(a_TexCoord);
    float px           = dot(dx, dx);
    float py           = dot(dy, dy);
    float maxLod       = 0.5 * log2(max(px, py)); // log2(sqrt()) = 0.5*log2()
    float minLod       = 0.5 * log2(min(px, py));
    float anisoLOD     = maxLod - min(maxLod - minLod, maxAnisoLog2);
    return max(anisoLOD, 0.0);
}

void main()
{
    beginInvocationInterlockARB();
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
        ivec2 texelCoord   = WrapTexelCoords(
            texInfo.wrapS, texInfo.wrapT,
            uvec2(texInfo.texSize),
            ivec2(transformedUV * texInfo.texSize));
        float desiredLod = round(ComputeLOD(texInfo.maxAniso, texelCoord));
        vec2 uv          = texelCoord / texInfo.texSize;
        // TODO WRAP UV
        ssbo_Output[texInfo.id].minUV  = min(ssbo_Output[texInfo.id].minUV, uv);
        ssbo_Output[texInfo.id].maxUV  = max(ssbo_Output[texInfo.id].maxUV, uv);
        ssbo_Output[texInfo.id].minMip = min(ssbo_Output[texInfo.id].minMip, desiredLod);
        ssbo_Output[texInfo.id].maxMip = max(ssbo_Output[texInfo.id].maxMip, desiredLod);
    }
    endInvocationInterlockARB();
}