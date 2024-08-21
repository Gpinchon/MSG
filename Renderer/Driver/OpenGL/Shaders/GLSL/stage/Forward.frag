//////////////////////////////////////// INCLUDES
#include <BRDF.glsl>
#include <Bindings.glsl>
#include <Functions.glsl>
#include <Material.glsl>
#include <ToneMapping.glsl>
#ifndef DEFERRED_LIGHTING
#include <Camera.glsl>
#include <SphericalHarmonics.glsl>
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
#ifndef DEFERRED_LIGHTING
 #if MATERIAL_ALPHA_MODE == MATERIAL_ALPHA_BLEND
 layout(location = OUTPUT_FRAG_FWD_BLENDED_ACCUM) out vec4 out_Accum;
 layout(location = OUTPUT_FRAG_FWD_BLENDED_REV) out float out_Revealage;
 layout(location = OUTPUT_FRAG_FWD_BLENDED_COLOR) out vec3 out_Modulate;
 #else
 layout(location = OUTPUT_FRAG_FWD_OPAQUE_COLOR) out vec4 out_Color;
 layout(location = OUTPUT_FRAG_FWD_OPAQUE_VELOCITY) out vec2 out_Velocity;
 #endif
#else
layout(location = OUTPUT_DFD_FRAG_MATERIAL) out uvec4 out_Material;
layout(location = OUTPUT_DFD_FRAG_NORMAL) out vec3 out_Normal;
layout(location = OUTPUT_DFD_FRAG_VELOCITY) out vec2 out_Velocity;
layout(location = OUTPUT_DFD_FRAG_FINAL) out vec4 out_Final;
#endif
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
vec3 FresnelSchlickRoughness(float a_CosTheta, vec3 F0, float a_Alpha)
{
    // See fresnelSchlick
    float fresnel = exp2((-5.55473 * a_CosTheta - 6.98316) * a_CosTheta);
    vec3 Fr       = max(vec3(1.0 - a_Alpha), F0) - F0;

    return Fr * fresnel + F0;
}

vec4[SAMPLERS_VTFS_IBL_COUNT] SampleTexturesIBL(IN(BRDF) a_BRDF, IN(vec3) a_R)
{
    vec4 textureSamples[SAMPLERS_VTFS_IBL_COUNT];
    for (uint i = 0; i < SAMPLERS_VTFS_IBL_COUNT; i++) {
        textureSamples[i] = sampleLod(u_IBLSamplers[i], a_R, pow(a_BRDF.alpha, 1.f / 2.f));
    }
    return textureSamples;
}

vec3 GetLightColor(IN(BRDF) a_BRDF, IN(vec3) a_WorldPosition, IN(vec3) a_Normal)
{
    const vec3 V                   = normalize(u_Camera.position - a_WorldPosition);
    vec3 N                   = a_Normal;
    float NdotV              = dot(N, V);
    if (NdotV < 0.f) {
        N     = -N;
        NdotV = dot(N, V);
    }
    const uvec3 vtfsClusterIndex   = VTFSClusterIndex(in_NDCPosition);
    const uint vtfsClusterIndex1D  = VTFSClusterIndexTo1D(vtfsClusterIndex);
    const uint lightCount          = vtfsClusters[vtfsClusterIndex1D].count;
    const vec3 R                   = reflect(V, N);
    const vec4 textureSamplesIBL[] = SampleTexturesIBL(a_BRDF, -R);
    const vec2 textureSampleBRDF   = texture(u_BRDFLut, vec2(NdotV, a_BRDF.alpha)).xy;
    vec3 totalLightColor           = vec3(0);
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
            const float NdotL             = saturate(dot(N, L));
            const vec3 specular           = GGXSpecular(a_BRDF, N, V, L);
            const vec3 lightParticipation = a_BRDF.cDiff * NdotL + specular;
            totalLightColor += lightParticipation * lightColor * lightAttenuation;
        } else if (lightType == LIGHT_TYPE_IBL) {
            const vec3 F             = FresnelSchlickRoughness(NdotV, a_BRDF.f0, a_BRDF.alpha);
            const vec3 lightSpecular = textureSamplesIBL[lightIBL[lightIndex].specularIndex].rgb;
            const vec3 diffuse       = a_BRDF.cDiff * SampleSH(lightIBL[lightIndex].irradianceCoefficients, N);
            const vec3 specular      = lightSpecular * (F * textureSampleBRDF.x + textureSampleBRDF.y);
            totalLightColor += (diffuse + specular) * lightColor * lightIntensity;
        }
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
    vec3 baseColor                = SRGBToLinear(a_TextureSamples[SAMPLERS_MATERIAL_METROUGH_COL].rgb);
    float metallic                = a_TextureSamples[SAMPLERS_MATERIAL_METROUGH_MR].b;
    float roughness               = a_TextureSamples[SAMPLERS_MATERIAL_METROUGH_MR].g;
    baseColor                     = baseColor * u_Material.colorFactor.rgb;
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

vec3 GetEmissive(IN(vec4) a_TextureSamples[SAMPLERS_MATERIAL_COUNT])
{
    return u_Material.base.emissiveFactor * a_TextureSamples[SAMPLERS_MATERIAL_BASE_EMISSIVE].rgb;
}

vec3 GetNormal(IN(vec4) a_TextureSamples[SAMPLERS_MATERIAL_COUNT])
{
    mat3 tbn    = transpose(mat3(normalize(in_WorldTangent), normalize(in_WorldBitangent), normalize(in_WorldNormal)));
    vec3 normal = a_TextureSamples[SAMPLERS_MATERIAL_BASE_NORMAL].rgb * 2 - 1;
    normal      = normal * vec3(vec2(u_Material.base.normalScale), 1);
    return normalize(normal * tbn);
}

#if MATERIAL_ALPHA_MODE == MATERIAL_ALPHA_BLEND
void WritePixel(IN(vec4) a_Color, IN(vec3) a_Transmition)
{
    float csZ = in_NDCPosition.z * 0.5 + 0.5;
    vec4 premultipliedReflect = vec4(a_Color.rgb * a_Color.a, a_Color.a);
    premultipliedReflect.a *= 1.0 - (a_Transmition.r + a_Transmition.g + a_Transmition.b) / 3.0;
    float tmp = (premultipliedReflect.a * 8.0 + 0.01) *
                 (-gl_FragCoord.z * 0.95 + 1.0);
    tmp /= sqrt(abs(csZ));
    float w = clamp(tmp * tmp * tmp * 1e3, 1e-2, 3e2);

    out_Accum               = premultipliedReflect * w;
    out_Revealage           = premultipliedReflect.a;
    out_Modulate            = a_Color.a * (vec3(1.f) - a_Transmition);
}
#endif //MATERIAL_ALPHA_MODE == MATERIAL_ALPHA_BLEND

void main()
{
    const vec4 textureSamples[] = SampleTextures();
    const BRDF brdf             = GetBRDF(textureSamples);
    const vec3 normal           = GetNormal(textureSamples);
    const vec3 emissive         = GetEmissive(textureSamples);
#ifndef DEFERRED_LIGHTING
    vec4 color = vec4(0, 0, 0, 1);
    color.rgb += GetLightColor(brdf, in_WorldPosition, normal);
    color.rgb += emissive;
    color.a = brdf.transparency;
#if MATERIAL_ALPHA_MODE == MATERIAL_ALPHA_BLEND
    if (color.a >= 1)
        discard;
    const vec3 transmit = brdf.cDiff * (1 - color.a);
    WritePixel(color, transmit);
#else
    if (color.a < u_Material.base.alphaCutoff)
        discard;
    out_Color = color;
#endif //MATERIAL_ALPHA_MODE == MATERIAL_ALPHA_BLEND
#else
    out_Final                   = vec4(0, 0, 0, 1);
    float AO        = 0;
    out_Material[0] = packUnorm4x8(vec4(brdf.cDiff, brdf.alpha));
    out_Material[1] = packUnorm4x8(vec4(brdf.f0, AO));
    out_Normal      = normal;
    out_Final.rgb += emissive;
#endif // DEFERRED_LIGHTING
}