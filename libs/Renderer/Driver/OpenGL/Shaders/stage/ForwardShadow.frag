#include <Bindings.glsl>
#include <Functions.glsl>
#include <Material.glsl>
#include <ToneMapping.glsl>

//////////////////////////////////////// STAGE INPUTS
layout(location = 0) in vec3 in_WorldPosition;
layout(location = 4) in vec2 in_TexCoord[ATTRIB_TEXCOORD_COUNT];
//////////////////////////////////////// STAGE INPUTS


void main()
{
    const vec4 textureSamplesMaterials[] = SampleTexturesMaterial();
    const BRDF brdf                      = GetBRDF(textureSamplesMaterials);
    const vec3 normal                    = GetNormal(textureSamplesMaterials);
    const vec3 emissive                  = GetEmissive(textureSamplesMaterials);
    const float occlusion                = GetOcclusion(textureSamplesMaterials);
#ifndef DEFERRED_LIGHTING
    vec4 color = vec4(0, 0, 0, 1);
#ifdef MATERIAL_UNLIT
    color.rgb += brdf.cDiff;
#else
    color.rgb += GetLightColor(brdf, in_WorldPosition, normal, occlusion);
#endif // MATERIAL_UNLIT
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
    out_Color    = color;
    vec3 a       = in_Position.xyz / in_Position.w * 0.5 + 0.5;
    vec3 b       = in_Position_Previous.xyz / in_Position_Previous.w * 0.5 + 0.5;
    out_Velocity = b.xy - a.xy;
#endif // MATERIAL_ALPHA_MODE == MATERIAL_ALPHA_BLEND
#else
    out_Final       = vec4(0, 0, 0, 1);
    float AO        = 0;
    out_Material[0] = packUnorm4x8(vec4(brdf.cDiff, brdf.alpha));
    out_Material[1] = packUnorm4x8(vec4(brdf.f0, AO));
    out_Normal      = normal;
    out_Final.rgb += emissive;
#endif // DEFERRED_LIGHTING
}