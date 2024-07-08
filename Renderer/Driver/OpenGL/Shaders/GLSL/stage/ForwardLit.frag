#include <Bindings.glsl>
#include <Material.glsl>
#include <VTFSLightSampling.glsl>

// uniform buffers
layout(binding = UBO_MATERIAL) uniform CommonMaterialBlock
{
    CommonMaterial u_CommonMaterial;
    TextureInfo u_TextureInfo[SAMPLERS_MATERIAL_COUNT];
};
layout(binding = UBO_MATERIAL) uniform MetallicRoughnessMaterialBlock
{
    MetallicRoughnessMaterial u_MetallicRoughnessMaterial;
};
layout(binding = UBO_MATERIAL) uniform SpecularGlossinessMaterialBlock
{
    SpecularGlossinessMaterial u_SpecularGlossinessMaterial;
};
// samplers
layout(binding = SAMPLERS_MATERIAL) uniform sampler2D u_MaterialSamplers[SAMPLERS_MATERIAL_COUNT];

layout(location = 0) in vec3 in_WorldPosition;
layout(location = 1) in vec3 in_WorldNormal;
layout(location = 2) in vec4 in_Tangent;
layout(location = 3) in vec4 in_Bitangent;
layout(location = 4) in vec2 in_TexCoord[ATTRIB_TEXCOORD_COUNT];
layout(location = 4 + ATTRIB_TEXCOORD_COUNT) in vec3 in_Color;
layout(location = 4 + ATTRIB_TEXCOORD_COUNT + 1) noperspective in vec3 in_NDCPosition;

out vec4 fragColor;

void main()
{
    vec4 textureSamples[SAMPLERS_MATERIAL_COUNT];
    for (uint i = 0; i < SAMPLERS_MATERIAL_COUNT; ++i) {
        const vec2 texCoord  = in_TexCoord[u_TextureInfo[i].texCoord];
        const vec2 scale     = u_TextureInfo[i].transform.scale;
        const vec2 offset    = u_TextureInfo[i].transform.offset;
        const float rotation = u_TextureInfo[i].transform.rotation;
        mat3 rotationMat     = mat3(
            cos(rotation), sin(rotation), 0,
            -sin(rotation), cos(rotation), 0,
            0, 0, 1);
        vec2 uvTransformed = (rotationMat * vec3(texCoord.xy, 1)).xy * scale + offset;
        textureSamples[i]  = texture(u_MaterialSamplers[i], uvTransformed);
    }
    vec3 normal = in_WorldNormal;
    {
        normal   = textureSamples[SAMPLERS_MATERIAL_BASE_NORMAL].rgb * 0.5 + 0.5;
        mat3 tbn = mat3(
            in_Tangent, in_Bitangent, in_WorldNormal);
        normal = tbn * normal;
    }

    uvec3 vtfsClusterIndex  = VTFSClusterIndex(in_NDCPosition);
    uint vtfsClusterIndex1D = VTFSClusterIndexTo1D(vtfsClusterIndex);
    uint lightCount         = vtfsClusters[vtfsClusterIndex1D].count;
    vec3 totalLightColor    = vec3(0);
    for (uint i = 0; i < lightCount; i++) {
        totalLightColor += SampleLight(
            in_WorldPosition,
            in_WorldNormal,
            vtfsClusters[vtfsClusterIndex1D].index[i]);
    }
    fragColor.rgb = totalLightColor;
#if (MATERIAL_TYPE == MATERIAL_TYPE_SPECULAR_GLOSSINESS)
    vec3 diffuse = textureSamples[SAMPLERS_MATERIAL_SPECGLOSS_DIFF].rgb;
    fragColor.rgb *= diffuse;
#endif //(MATERIAL_TYPE == MATERIAL_TYPE_SPECULAR_GLOSSINESS)
    fragColor.a = 1;
}