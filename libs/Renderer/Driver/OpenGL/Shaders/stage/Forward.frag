//////////////////////////////////////// INCLUDES
#include <BRDF.glsl>
#include <Bindings.glsl>
#include <Camera.glsl>
#include <FrameInfo.glsl>
#include <Functions.glsl>
#include <FwdLights.glsl>
#include <MaterialInputs.glsl>
#include <VTFSLightSampling.glsl>
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
layout(binding = UBO_FRAME_INFO) uniform FrameInfoBlock
{
    FrameInfo u_FrameInfo;
};
layout(binding = UBO_CAMERA) uniform CameraBlock
{
    Camera u_Camera;
    Camera u_Camera_Previous;
};
//////////////////////////////////////// UNIFORMS

vec3 GetLightColor(IN(BRDF) a_BRDF, IN(vec3) a_WorldPosition, IN(vec3) a_Normal, IN(float) a_Occlusion)
{
    vec3 totalLightColor = vec3(0);
    const vec3 V         = normalize(u_Camera.position - a_WorldPosition);
    vec3 N               = gl_FrontFacing ? a_Normal : -a_Normal;
    float NdotV          = dot(N, V);
    totalLightColor += GetVTFSLightColor(a_BRDF, a_WorldPosition, in_NDCPosition, N, V);
    totalLightColor += GetShadowLightColor(a_BRDF, a_WorldPosition, N, V, u_FrameInfo.frameIndex);
    totalLightColor += GetIBLColor(a_BRDF, SampleBRDFLut(a_BRDF, NdotV), a_WorldPosition, a_Occlusion, N, V, NdotV);
    return totalLightColor;
}

#if MATERIAL_ALPHA_MODE == MATERIAL_ALPHA_BLEND
void WritePixel(IN(vec4) a_Color, IN(vec3) a_Transmition)
{
    float csZ                 = in_NDCPosition.z * 0.5 + 0.5;
    vec4 premultipliedReflect = vec4(a_Color.rgb * a_Color.a, a_Color.a);
    premultipliedReflect.a *= 1.0 - (a_Transmition.r + a_Transmition.g + a_Transmition.b) / 3.0;
    float tmp = (premultipliedReflect.a * 8.0 + 0.01) * (-gl_FragCoord.z * 0.95 + 1.0);
    tmp /= sqrt(abs(csZ));
    float w = clamp(tmp * tmp * tmp * 1e3, 1e-2, 3e2);

    out_Accum     = premultipliedReflect * w;
    out_Revealage = premultipliedReflect.a;
    out_Modulate  = a_Color.a * (vec3(1.f) - a_Transmition);
}
#endif // MATERIAL_ALPHA_MODE == MATERIAL_ALPHA_BLEND

void main()
{
    const vec4 textureSamplesMaterials[] = SampleTexturesMaterial(in_TexCoord);
    const BRDF brdf                      = GetBRDF(textureSamplesMaterials, in_Color);
    const vec3 emissive                  = GetEmissive(textureSamplesMaterials);
    vec4 color                           = vec4(0, 0, 0, 1);

#if MATERIAL_UNLIT
    color.rgb += brdf.cDiff;
    color.rgb += emissive;
    color.a = brdf.transparency;
    return;
#else
    const float occlusion = GetOcclusion(textureSamplesMaterials);
    const vec3 normal     = GetNormal(textureSamplesMaterials, in_WorldTangent, in_WorldBitangent, in_WorldNormal);
    color.rgb += GetLightColor(brdf, in_WorldPosition, normal, occlusion);
    color.rgb += emissive;
    color.a = brdf.transparency;
#endif // MATERIAL_UNLIT

#if MATERIAL_ALPHA_MODE == MATERIAL_ALPHA_BLEND
    if (color.a >= 1)
        discard;
    const vec3 transmit = brdf.cDiff * (1 - color.a);
    WritePixel(color, transmit);
    return;
#else
    if (color.a < u_Material.base.alphaCutoff)
        discard;
    out_Color    = color;
    vec3 a       = in_Position.xyz / in_Position.w * 0.5 + 0.5;
    vec3 b       = in_Position_Previous.xyz / in_Position_Previous.w * 0.5 + 0.5;
    out_Velocity = b.xy - a.xy;
#endif // MATERIAL_ALPHA_MODE == MATERIAL_ALPHA_BLEND
}