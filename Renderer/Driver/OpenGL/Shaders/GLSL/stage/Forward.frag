//////////////////////////////////////// INCLUDES
#include <Bindings.glsl>
#include <Functions.glsl>
#include <Material.glsl>
#ifndef DEFERRED_LIGHTING
#include <Camera.glsl>
#include <VTFSLightSampling.glsl>
#endif // DEFERRED_LIGHTING
//////////////////////////////////////// INCLUDES

//////////////////////////////////////// STAGE INPUTS
layout(location = 0) in vec3 in_WorldPosition;
layout(location = 1) in vec3 in_WorldNormal;
layout(location = 2) in vec3 in_WorldTangent;
layout(location = 3) in vec3 in_WorldBitangent;
layout(location = 4) in vec2 in_TexCoord[ATTRIB_TEXCOORD_COUNT];
layout(location = 4 + ATTRIB_TEXCOORD_COUNT) in vec3 in_Color;
layout(location = 4 + ATTRIB_TEXCOORD_COUNT + 1) noperspective in vec3 in_NDCPosition;
//////////////////////////////////////// STAGE INPUTS

//////////////////////////////////////// STAGE OUTPUTS
layout(location = OUTPUT_FRAG_MATERIAL) out uvec4 out_Material;
layout(location = OUTPUT_FRAG_NORMAL) out vec3 out_Normal;
layout(location = OUTPUT_FRAG_VELOCITY) out vec2 out_Velocity;
layout(location = OUTPUT_FRAG_FINAL) out vec4 out_Final;
//////////////////////////////////////// STAGE OUTPUTS

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
#ifndef DEFERRED_LIGHTING
layout(binding = UBO_CAMERA) uniform CameraBlock
{
    Camera u_Camera;
};
layout(binding = SAMPLERS_BRDF_LUT) uniform sampler2D u_BRDFLut;
#endif // DEFERRED_LIGHTING
//////////////////////////////////////// UNIFORMS
#ifndef DEFERRED_LIGHTING
vec3 ggx(vec3 N, vec3 V, vec3 L, float alpha, vec3 F0)
{
    alpha                = 1 - alpha;
    const vec3 H         = normalize(L - V);
    const float dotLH    = saturate(dot(L, H));
    const float dotNH    = saturate(dot(N, H));
    const float dotNL    = saturate(dot(N, L));
    const float alphaSqr = (alpha * alpha);
    const float denom    = dotNH * dotNH * (alphaSqr - 1.0) + 1.0;
    const float D        = alphaSqr / (3.141592653589793 * denom * denom);
    const vec3 F         = F0 + (1.0 - F0) * pow(1.0 - dotLH, 5.0);
    const float k        = 0.5 * alpha;
    const float k2       = k * k;
    return dotNL * D * F / (dotLH * dotLH * (1.0 - k2) + k2);
}

vec3 GetLightColor(IN(BRDF) a_BRDF, IN(vec3) a_WorldPosition, IN(vec3) a_Normal)
{
    const uvec3 vtfsClusterIndex  = VTFSClusterIndex(in_NDCPosition);
    const uint vtfsClusterIndex1D = VTFSClusterIndexTo1D(vtfsClusterIndex);
    const uint lightCount         = vtfsClusters[vtfsClusterIndex1D].count;
    const vec3 V                  = normalize(u_Camera.position - a_WorldPosition);
    const vec3 N                  = a_Normal;
    vec3 totalLightColor          = vec3(0);
    for (uint i = 0; i < lightCount; i++) {
        const uint lightIndex      = vtfsClusters[vtfsClusterIndex1D].index[i];
        const int lightType        = lightBase[lightIndex].commonData.type;
        const vec3 lightPosition   = lightBase[lightIndex].commonData.position;
        const vec3 lightColor      = lightBase[lightIndex].commonData.color;
        const float lightRange     = lightBase[lightIndex].commonData.range;
        const float lightIntensity = lightBase[lightIndex].commonData.intensity;
        const float lightFalloff   = lightBase[lightIndex].commonData.falloff;
        float lightAttenuation     = 0;
        vec3 L                     = vec3(0);
        if (lightType == LIGHT_TYPE_POINT || lightType == LIGHT_TYPE_SPOT) {
            L                 = (lightPosition - a_WorldPosition);
            const float LDist = length(L);
            L                 = normalize(L);
            lightAttenuation  = PointLightAttenuation(LDist, lightRange, lightIntensity, lightFalloff);
            if (lightType == LIGHT_TYPE_SPOT) {
                const vec3 lightDir             = lightSpot[lightIndex].direction;
                const float lightInnerConeAngle = lightSpot[lightIndex].innerConeAngle;
                const float lightOuterConeAngle = lightSpot[lightIndex].outerConeAngle;
                lightAttenuation *= SpotLightAttenuation(L, lightDir, lightInnerConeAngle, lightOuterConeAngle);
            }
        }
        const float NdotL             = saturate(dot(N, L));
        const vec3 specular           = ggx(N, V, L, a_BRDF.alpha, a_BRDF.f0);
        const vec3 lightParticipation = a_BRDF.cDiff * NdotL + specular;
        totalLightColor += lightParticipation * lightColor * lightAttenuation;
    }
    return totalLightColor;
}
#endif // DEFERRED_LIGHTING

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
    mat3 tbn    = transpose(mat3(normalize(in_WorldTangent), normalize(in_WorldBitangent), normalize(in_WorldNormal)));
    vec3 normal = (a_TextureSamples[SAMPLERS_MATERIAL_BASE_NORMAL].rgb * 2 - 1) * tbn;
    return normalize(normal);
}

void main()
{
    out_Final                   = vec4(0);
    const vec4 textureSamples[] = SampleTextures();
    const BRDF brdf             = GetBRDF(textureSamples);
    const vec3 normal           = GetNormal(textureSamples);
    const vec3 emissive         = GetEmissive(textureSamples);
#ifndef DEFERRED_LIGHTING
    out_Final.rgb += GetLightColor(brdf, in_WorldPosition, normal);
    out_Final.rgb += emissive;
#else
    float AO        = 0;
    out_Material[0] = packUnorm4x8(vec4(brdf.cDiff, brdf.alpha));
    out_Material[1] = packUnorm4x8(vec4(brdf.f0, AO));
    out_Normal      = normal;
    out_Final.rgb += emissive;
#endif // DEFERRED_LIGHTING
}