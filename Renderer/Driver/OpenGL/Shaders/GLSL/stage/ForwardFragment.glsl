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
layout(location = 10) noperspective in vec3 in_NDCPosition;

out vec4 fragColor;

void main()
{
    vec4 textureSamples[SAMPLERS_MATERIAL_COUNT];
    for (uint i = 0; i < SAMPLERS_MATERIAL_COUNT; ++i) {
        const vec2 texCoord = in_TexCoord[u_TextureInfo[i].texCoord];
        textureSamples[i]   = texture(u_MaterialSamplers[i], texCoord);
    }
    vec3 normal = in_WorldNormal;
    {
        normal   = textureSamples[SAMPLERS_MATERIAL_BASE_NORMAL].rgb * 0.5 + 0.5;
        mat3 tbn = mat3(
            in_Tangent, in_Bitangent, in_WorldNormal);
        normal = tbn * normal;
    }
    vec3 diffuse            = textureSamples[SAMPLERS_MATERIAL_SPECGLOSS_DIFF].rgb;
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
    fragColor.rgb = diffuse * totalLightColor;
    fragColor.a   = 1;
}