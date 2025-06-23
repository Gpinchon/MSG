#ifndef MATERIAL_INPUTS_GLSL
#define MATERIAL_INPUTS_GLSL
#ifndef __cplusplus

#include <BRDF.glsl>
#include <Bindings.glsl>
#include <Material.glsl>
#include <ToneMapping.glsl>
#include <VirtualTexturing.glsl>

//////////////////////////////////////// UNIFORMS
layout(binding = UBO_MATERIAL) uniform CommonMaterialBlock
{
    CommonMaterial u_CommonMaterial;
    TextureInfo u_TextureInfo[SAMPLERS_MATERIAL_COUNT];
};
layout(binding = UBO_MATERIAL) uniform MaterialBlock
{
#if (MATERIAL_TYPE == MATERIAL_TYPE_METALLIC_ROUGHNESS)
    MetallicRoughnessMaterial u_Material;
#elif (MATERIAL_TYPE == MATERIAL_TYPE_SPECULAR_GLOSSINESS)
    SpecularGlossinessMaterial u_Material;
#endif //(MATERIAL_TYPE == MATERIAL_TYPE_SPECULAR_GLOSSINESS)
};
layout(binding = SAMPLERS_MATERIAL) uniform sampler2D u_MaterialSamplers[SAMPLERS_MATERIAL_COUNT];
//////////////////////////////////////// UNIFORMS

#if MATERIAL_UNLIT
BRDF GetBRDF(IN(vec4) a_TextureSamples[SAMPLERS_MATERIAL_COUNT], IN(vec3) a_Color)
{
    BRDF brdf;
#if (MATERIAL_TYPE == MATERIAL_TYPE_METALLIC_ROUGHNESS)
    brdf.cDiff        = SRGBToLinear(a_TextureSamples[SAMPLERS_MATERIAL_METROUGH_COL].rgb) * u_Material.colorFactor.rgb * a_Color;
    brdf.transparency = u_Material.colorFactor.a * a_TextureSamples[SAMPLERS_MATERIAL_METROUGH_COL].a;
#elif (MATERIAL_TYPE == MATERIAL_TYPE_SPECULAR_GLOSSINESS)
    brdf.cDiff        = SRGBToLinear(a_TextureSamples[SAMPLERS_MATERIAL_SPECGLOSS_DIFF].rgb) * u_Material.diffuseFactor.rgb * a_Color;
    brdf.transparency = u_Material.diffuseFactor.a * a_TextureSamples[SAMPLERS_MATERIAL_SPECGLOSS_DIFF].a;
#endif //(MATERIAL_TYPE == MATERIAL_TYPE_SPECULAR_GLOSSINESS)
    return brdf;
}
#else
BRDF GetBRDF(IN(vec4) a_TextureSamples[SAMPLERS_MATERIAL_COUNT], IN(vec3) a_Color)
{
    BRDF brdf;
#if (MATERIAL_TYPE == MATERIAL_TYPE_METALLIC_ROUGHNESS)
    const vec3 dielectricSpecular = vec3(0.04);
    const vec3 black              = vec3(0);
    vec3 baseColor                = SRGBToLinear(a_TextureSamples[SAMPLERS_MATERIAL_METROUGH_COL].rgb);
    float metallic                = a_TextureSamples[SAMPLERS_MATERIAL_METROUGH_MR].b;
    float roughness               = a_TextureSamples[SAMPLERS_MATERIAL_METROUGH_MR].g;
    baseColor                     = baseColor * u_Material.colorFactor.rgb * a_Color;
    metallic                      = metallic * u_Material.metallicFactor;
    roughness                     = roughness * u_Material.roughnessFactor;
    brdf.transparency             = u_Material.colorFactor.a * a_TextureSamples[SAMPLERS_MATERIAL_METROUGH_COL].a;
    brdf.cDiff                    = mix(baseColor * (1 - dielectricSpecular.r), black, metallic);
    brdf.f0                       = mix(dielectricSpecular, baseColor, metallic);
    brdf.alpha                    = roughness * roughness;
#elif (MATERIAL_TYPE == MATERIAL_TYPE_SPECULAR_GLOSSINESS)
    vec3 diffuse      = SRGBToLinear(a_TextureSamples[SAMPLERS_MATERIAL_SPECGLOSS_DIFF].rgb);
    vec3 specular     = SRGBToLinear(a_TextureSamples[SAMPLERS_MATERIAL_SPECGLOSS_SG].rgb);
    float glossiness  = a_TextureSamples[SAMPLERS_MATERIAL_SPECGLOSS_SG].a;
    diffuse           = diffuse * u_Material.diffuseFactor.rgb;
    specular          = specular * u_Material.specularFactor;
    glossiness        = glossiness * u_Material.glossinessFactor;
    brdf.transparency = u_Material.diffuseFactor.a * a_TextureSamples[SAMPLERS_MATERIAL_SPECGLOSS_DIFF].a;
    brdf.cDiff        = diffuse.rgb * (1 - compMax(specular));
    brdf.f0           = specular;
    brdf.alpha        = pow(1 - glossiness, 2);
#endif //(MATERIAL_TYPE == MATERIAL_TYPE_SPECULAR_GLOSSINESS)
    return brdf;
}

float GetOcclusion(IN(vec4) a_TextureSamples[SAMPLERS_MATERIAL_COUNT])
{
    return 1 + u_Material.base.occlusionStrength * (a_TextureSamples[SAMPLERS_MATERIAL_BASE_OCCLUSION].r - 1);
}

vec3 GetNormal(IN(vec4) a_TextureSamples[SAMPLERS_MATERIAL_COUNT], IN(vec3) a_Tangent, IN(vec3) a_Bitangent, IN(vec3) a_Normal)
{
    mat3 tbn    = transpose(mat3(normalize(a_Tangent), normalize(a_Bitangent), normalize(a_Normal)));
    vec3 normal = a_TextureSamples[SAMPLERS_MATERIAL_BASE_NORMAL].rgb * 2 - 1;
    normal      = normal * vec3(vec2(u_Material.base.normalScale), 1);
    return normalize(normal * tbn);
}
#endif // MATERIAL_UNLIT

vec3 GetEmissive(IN(vec4) a_TextureSamples[SAMPLERS_MATERIAL_COUNT])
{
    return u_Material.base.emissiveFactor * a_TextureSamples[SAMPLERS_MATERIAL_BASE_EMISSIVE].rgb;
}

vec4[SAMPLERS_MATERIAL_COUNT] SampleTexturesMaterial(IN(vec2) a_TexCoords[ATTRIB_TEXCOORD_COUNT])
{
    vec4 textureSamplesMaterials[SAMPLERS_MATERIAL_COUNT];
    for (uint i = 0; i < SAMPLERS_MATERIAL_COUNT; ++i) {
        const vec2 texCoord  = a_TexCoords[u_TextureInfo[i].texCoord];
        const vec2 scale     = u_TextureInfo[i].transform.scale;
        const vec2 offset    = u_TextureInfo[i].transform.offset;
        const float rotation = u_TextureInfo[i].transform.rotation;
        mat3 rotationMat     = mat3(
            cos(rotation), sin(rotation), 0,
            -sin(rotation), cos(rotation), 0,
            0, 0, 1);
        vec2 uvTransformed = (rotationMat * vec3(texCoord.xy, 1)).xy * scale + offset;
        vec4 outColor      = vec4(1);
        uint maxLod        = textureQueryLevels(u_MaterialSamplers[i]);
        float lod          = min(VTComputeLOD(uvTransformed), maxLod - 1);
        int residencyCode  = sparseTextureLodARB(u_MaterialSamplers[i], uvTransformed, lod, outColor);
        for (;
            (lod < maxLod) && !sparseTexelsResidentARB(residencyCode);
            lod += 1) {
            residencyCode = sparseTextureLodARB(u_MaterialSamplers[i], uvTransformed, lod, outColor);
        }
        textureSamplesMaterials[i] = outColor;
    }
    return textureSamplesMaterials;
}

vec4[SAMPLERS_MATERIAL_COUNT] SampleTexturesMaterialLod(IN(vec2) a_TexCoords[ATTRIB_TEXCOORD_COUNT], IN(float) a_Lod)
{
    vec4 textureSamplesMaterials[SAMPLERS_MATERIAL_COUNT];
    for (uint i = 0; i < SAMPLERS_MATERIAL_COUNT; ++i) {
        const vec2 texCoord  = a_TexCoords[u_TextureInfo[i].texCoord];
        const vec2 scale     = u_TextureInfo[i].transform.scale;
        const vec2 offset    = u_TextureInfo[i].transform.offset;
        const float rotation = u_TextureInfo[i].transform.rotation;
        mat3 rotationMat     = mat3(
            cos(rotation), sin(rotation), 0,
            -sin(rotation), cos(rotation), 0,
            0, 0, 1);
        vec2 uvTransformed = (rotationMat * vec3(texCoord.xy, 1)).xy * scale + offset;
        vec4 outColor      = vec4(1);
        uint maxLod        = textureQueryLevels(u_MaterialSamplers[i]);
        float lod          = min(a_Lod, maxLod - 1);
        int residencyCode  = sparseTextureLodARB(u_MaterialSamplers[i], uvTransformed, lod, outColor);
        for (; (lod < maxLod) && !sparseTexelsResidentARB(residencyCode); lod += 1) {
            residencyCode = sparseTextureLodARB(u_MaterialSamplers[i], uvTransformed, lod, outColor);
        }
        textureSamplesMaterials[i] = outColor;
    }
    return textureSamplesMaterials;
}
#endif //__cplusplus
#endif MATERIAL_INPUTS_GLSL
