#include <Bindings.glsl>
#include <Material.glsl>
#if (DEFERRED_LIGHTING == false)
#include <VTFSLightSampling.glsl>
#endif //(DEFERRED_LIGHTING == false)

// uniform buffers
layout(binding = UBO_MATERIAL) uniform CommonMaterialBlock
{
    CommonMaterial u_CommonMaterial;
    TextureInfo u_TextureInfo[SAMPLERS_MATERIAL_COUNT];
};
#if (MATERIAL_TYPE == MATERIAL_TYPE_METALLIC_ROUGHNESS)
layout(binding = UBO_MATERIAL) uniform MaterialBlock
{
    MetallicRoughnessMaterial u_Material;
};
#elif (MATERIAL_TYPE == MATERIAL_TYPE_SPECULAR_GLOSSINESS)
layout(binding = UBO_MATERIAL) uniform MaterialBlock
{
    SpecularGlossinessMaterial u_Material;
};
#endif //(MATERIAL_TYPE == MATERIAL_TYPE_SPECULAR_GLOSSINESS)
layout(binding = SAMPLERS_MATERIAL) uniform sampler2D u_MaterialSamplers[SAMPLERS_MATERIAL_COUNT];

layout(location = 0) in vec3 in_WorldPosition;
layout(location = 1) in vec3 in_WorldNormal;
layout(location = 2) in vec4 in_Tangent;
layout(location = 3) in vec4 in_Bitangent;
layout(location = 4) in vec2 in_TexCoord[ATTRIB_TEXCOORD_COUNT];
layout(location = 4 + ATTRIB_TEXCOORD_COUNT) in vec3 in_Color;
layout(location = 4 + ATTRIB_TEXCOORD_COUNT + 1) noperspective in vec3 in_NDCPosition;

#if (DEFERRED_LIGHTING == false)
layout(location = OUTPUT_FRAG_FINAL) out vec4 out_Final;
#else
layout(location = OUTPUT_FRAG_CDIFF_F0_APLHA_AO) out uvec4 out_CDiff_F0_Alpha_AO;
layout(location = OUTPUT_FRAG_NORMAL) out vec3 out_Normal;
layout(location = OUTPUT_FRAG_VELOCITY) out vec2 out_Velocity;
layout(location = OUTPUT_FRAG_FINAL) out vec4 out_Final;
#endif //(DEFERRED_LIGHTING == false)

vec4[SAMPLERS_MATERIAL_COUNT] SampleTextures()
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
    return textureSamples;
}

BRDF GetBRDF(IN(vec4) a_TextureSamples[SAMPLERS_MATERIAL_COUNT])
{
    BRDF brdf;
#if (MATERIAL_TYPE == MATERIAL_TYPE_METALLIC_ROUGHNESS)
    const vec3 dielectricSpecular = vec3(0.04);
    const vec3 black              = vec3(0);
    vec3 baseColor                = a_TextureSamples[SAMPLERS_MATERIAL_METROUGH_COL].rgb;
    float metallic                = a_TextureSamples[SAMPLERS_MATERIAL_METROUGH_MR].r;
    float roughness               = a_TextureSamples[SAMPLERS_MATERIAL_METROUGH_MR].g;
    baseColor                     = baseColor * u_Material.colorFactor.rgb;
    metallic                      = metallic * u_Material.metallicFactor;
    roughness                     = roughness * u_Material.roughnessFactor;
    brdf.cDiff                    = mix(baseColor * (1 - dielectricSpecular.r), black, metallic);
    brdf.f0                       = mix(dielectricSpecular, baseColor, metallic);
    brdf.alpha                    = roughness * roughness;
#elif (MATERIAL_TYPE == MATERIAL_TYPE_SPECULAR_GLOSSINESS)
    vec3 diffuse     = a_TextureSamples[SAMPLERS_MATERIAL_SPECGLOSS_DIFF].rgb;
    vec3 specular    = a_TextureSamples[SAMPLERS_MATERIAL_SPECGLOSS_SG].rgb;
    float glossiness = a_TextureSamples[SAMPLERS_MATERIAL_SPECGLOSS_SG].a;
    diffuse          = diffuse * u_Material.diffuseFactor.rgb;
    specular         = specular * u_Material.specularFactor;
    glossiness       = glossiness * u_Material.glossinessFactor;
    brdf.cDiff       = diffuse.rgb * (1 - compMax(specular));
    brdf.f0          = specular;
    brdf.alpha       = pow(1 - glossiness, 2);
#endif //(MATERIAL_TYPE == MATERIAL_TYPE_SPECULAR_GLOSSINESS)
    return brdf;
}

vec3 GetEmissive(IN(vec4) a_TextureSamples[SAMPLERS_MATERIAL_COUNT])
{
    return u_Material.base.emissiveFactor * a_TextureSamples[SAMPLERS_MATERIAL_BASE_EMISSIVE].rgb;
}

vec3 GetNormal(IN(vec4) a_TextureSamples[SAMPLERS_MATERIAL_COUNT])
{
    mat3 tbn = mat3(
        in_Tangent, in_Bitangent, in_WorldNormal);
    vec3 normal = tbn * (a_TextureSamples[SAMPLERS_MATERIAL_BASE_NORMAL].rgb * 0.5 + 0.5);
    return normal;
}

#if (DEFERRED_LIGHTING == false)
vec3 GetLightColor()
{
    const uvec3 vtfsClusterIndex  = VTFSClusterIndex(in_NDCPosition);
    const uint vtfsClusterIndex1D = VTFSClusterIndexTo1D(vtfsClusterIndex);
    const uint lightCount         = vtfsClusters[vtfsClusterIndex1D].count;
    vec3 totalLightColor          = vec3(0);
    for (uint i = 0; i < lightCount; i++) {
        totalLightColor += SampleLight(
            in_WorldPosition,
            in_WorldNormal,
            vtfsClusters[vtfsClusterIndex1D].index[i]);
    }
    return totalLightColor;
}
#endif //(DEFERRED_LIGHTING == false)

void main()
{
    out_Final                   = vec4(0);
    const vec4 textureSamples[] = SampleTextures();
    const BRDF brdf             = GetBRDF(textureSamples);
    const vec3 normal           = GetNormal(textureSamples);
    const vec3 emissive         = GetEmissive(textureSamples);
#if (DEFERRED_LIGHTING == false)
    out_Final.rgb += GetLightColor() * brdf.cDiff;
    out_Final.rgb += emissive;
#else
    float AO        = 0;
    out_CDiff_AO[0] = packUnorm4x8(vec4(brdf.cDiff, brdf.alpha));
    out_CDiff_AO[1] = packUnorm4x8(vec4(brdf.f0, AO));
    out_Normal      = normal;
    out_Final.rgb += emissive;
#endif //(DEFERRED_LIGHTING == false)
}