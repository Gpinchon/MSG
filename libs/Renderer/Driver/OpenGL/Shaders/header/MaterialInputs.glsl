#ifndef MATERIAL_INPUTS_GLSL
#define MATERIAL_INPUTS_GLSL
#ifndef __cplusplus

#include <BRDF.glsl>
#include <Bindings.glsl>
#include <Material.glsl>
#include <Random.glsl>
#include <ToneMapping.glsl>
#include <VirtualTexturing.glsl>

//////////////////////////////////////// UNIFORMS
layout(binding = UBO_MATERIAL) uniform CommonMaterialBlock
{
    CommonMaterial u_CommonMaterial;
    VTInfo u_TextureInfo[SAMPLERS_MATERIAL_COUNT];
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
layout(binding = SAMPLERS_MATERIAL_PAGE_TABLE) uniform usampler2D u_MaterialSamplersPageTable[SAMPLERS_MATERIAL_COUNT];
//////////////////////////////////////// UNIFORMS

float GetTransparency(IN(vec4) a_CDiffSample)
{
#if (MATERIAL_TYPE == MATERIAL_TYPE_METALLIC_ROUGHNESS)
    float alphaVal = u_Material.colorFactor.a * a_CDiffSample.a;
#elif (MATERIAL_TYPE == MATERIAL_TYPE_SPECULAR_GLOSSINESS)
    float alphaVal = u_Material.diffuseFactor.a * a_CDiffSample.a;
#endif
    if (u_Material.base.alphaMode == MATERIAL_ALPHA_OPAQUE)
        return 1;
    else if (u_Material.base.alphaMode == MATERIAL_ALPHA_CUTOFF)
        return step(u_Material.base.alphaCutoff, alphaVal);
    else
        return alphaVal;
}

float GetTransparency(IN(vec4) a_TextureSamples[SAMPLERS_MATERIAL_COUNT])
{
#if (MATERIAL_TYPE == MATERIAL_TYPE_METALLIC_ROUGHNESS)
    return GetTransparency(a_TextureSamples[SAMPLERS_MATERIAL_METROUGH_COL]);
#elif (MATERIAL_TYPE == MATERIAL_TYPE_SPECULAR_GLOSSINESS)
    return GetTransparency(a_TextureSamples[SAMPLERS_MATERIAL_SPECGLOSS_DIFF]);
#endif
}

#if MATERIAL_UNLIT

BRDF GetBRDF(IN(vec4) a_TextureSamples[SAMPLERS_MATERIAL_COUNT], IN(vec3) a_Color)
{
    BRDF brdf;
#if (MATERIAL_TYPE == MATERIAL_TYPE_METALLIC_ROUGHNESS)
    brdf.cDiff = SRGBToLinear(a_TextureSamples[SAMPLERS_MATERIAL_METROUGH_COL].rgb) * u_Material.colorFactor.rgb * a_Color;
#elif (MATERIAL_TYPE == MATERIAL_TYPE_SPECULAR_GLOSSINESS)
    brdf.cDiff = SRGBToLinear(a_TextureSamples[SAMPLERS_MATERIAL_SPECGLOSS_DIFF].rgb) * u_Material.diffuseFactor.rgb * a_Color;
#endif //(MATERIAL_TYPE == MATERIAL_TYPE_SPECULAR_GLOSSINESS)
    brdf.transparency = GetTransparency(a_TextureSamples);
    return brdf;
}

#else // MATERIAL_UNLIT

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
    brdf.transparency             = GetTransparency(a_TextureSamples[SAMPLERS_MATERIAL_METROUGH_COL]);
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
    brdf.transparency = GetTransparency(a_TextureSamples[SAMPLERS_MATERIAL_SPECGLOSS_DIFF]);
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

vec2 TransformUVMaterial(IN(vec2) a_TexCoords[ATTRIB_TEXCOORD_COUNT], IN(uint) a_TextureIndex)
{
    VTInfo texInfo       = u_TextureInfo[a_TextureIndex];
    const vec2 texCoord  = a_TexCoords[texInfo.texCoord];
    const vec2 scale     = texInfo.transform.scale;
    const vec2 offset    = texInfo.transform.offset;
    const float rotation = texInfo.transform.rotation;
    mat3 rotationMat     = mat3(
        cos(rotation), sin(rotation), 0,
        -sin(rotation), cos(rotation), 0,
        0, 0, 1);
    return (rotationMat * vec3(texCoord.xy, 1)).xy * scale + offset;
}

vec4 SampleTextureMaterial(IN(vec2) a_TexCoords[ATTRIB_TEXCOORD_COUNT], IN(uint) a_TextureIndex)
{
    VTInfo texInfo     = u_TextureInfo[a_TextureIndex];
    vec2 transformedTC = TransformUVMaterial(a_TexCoords, a_TextureIndex) * texInfo.texSize;
    vec2 wrappedUV     = WrapTexelCoords(
                             texInfo.wrapS, texInfo.wrapT, texInfo.texSize,
                             transformedTC)
        / texInfo.texSize;
    float ditherVal = Dither(ivec2(transformedTC));
    float lod       = VTQueryLod(texInfo, wrappedUV);
    lod             = mix(floor(lod), ceil(lod), fract(lod) > ditherVal);
    uvec4 page      = textureLod(u_MaterialSamplersPageTable[a_TextureIndex], wrappedUV, lod);
    // uvec4 page = uvec4(texInfo.virtualLevels);
    if (lod >= int(texInfo.virtualLevels))
        page[2] = int(lod);
    return textureLod(u_MaterialSamplers[a_TextureIndex], wrappedUV, page[2]);
}

vec4 SampleCDiffMaterial(IN(vec2) a_TexCoords[ATTRIB_TEXCOORD_COUNT])
{
#if (MATERIAL_TYPE == MATERIAL_TYPE_METALLIC_ROUGHNESS)
    return SampleTextureMaterial(a_TexCoords, SAMPLERS_MATERIAL_METROUGH_COL);
#elif (MATERIAL_TYPE == MATERIAL_TYPE_SPECULAR_GLOSSINESS)
    return SampleTextureMaterial(a_TexCoords, SAMPLERS_MATERIAL_SPECGLOSS_DIFF);
#endif
    return vec4(0, 0, 0, 1);
}

vec4[SAMPLERS_MATERIAL_COUNT] SampleTexturesMaterial(IN(vec2) a_TexCoords[ATTRIB_TEXCOORD_COUNT])
{
    vec4 textureSamplesMaterials[SAMPLERS_MATERIAL_COUNT];
    textureSamplesMaterials[0] = SampleTextureMaterial(a_TexCoords, 0);
    textureSamplesMaterials[1] = SampleTextureMaterial(a_TexCoords, 1);
    textureSamplesMaterials[2] = SampleTextureMaterial(a_TexCoords, 2);
    textureSamplesMaterials[3] = SampleTextureMaterial(a_TexCoords, 3);
    textureSamplesMaterials[4] = SampleTextureMaterial(a_TexCoords, 4);
    textureSamplesMaterials[5] = SampleTextureMaterial(a_TexCoords, 5);
    textureSamplesMaterials[6] = SampleTextureMaterial(a_TexCoords, 6);
    return textureSamplesMaterials;
}
#endif //__cplusplus
#endif MATERIAL_INPUTS_GLSL
