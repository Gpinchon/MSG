//////////////////////////////////////// INCLUDES
#include <BRDFInputs.glsl>
#include <Bindings.glsl>
#include <Camera.glsl>
#include <FogInputs.glsl>
#include <FrameInfo.glsl>
#include <Functions.glsl>
#include <LightsIBLInputs.glsl>
#include <LightsShadowInputs.glsl>
#include <LightsVTFSInputs.glsl>
#include <MaterialInputs.glsl>
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
#if MATERIAL_ALPHA_MODE != MATERIAL_ALPHA_BLEND
layout(location = OUTPUT_FRAG_FWD_OPAQUE_COLOR) out vec4 out_Color;
layout(location = OUTPUT_FRAG_FWD_OPAQUE_VELOCITY) out vec2 out_Velocity;
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

vec3 GetLightColor(IN(BRDF) a_BRDF, IN(vec3) a_WorldPosition, IN(vec3) a_Normal)
{
    vec3 totalLightColor = vec3(0);
    const vec3 V         = normalize(u_Camera.position - a_WorldPosition);
    vec3 N               = gl_FrontFacing ? a_Normal : -a_Normal;
    float NdotV          = dot(N, V);
    totalLightColor += GetVTFSLightColor(a_BRDF, a_WorldPosition, in_NDCPosition, N, V);
    totalLightColor += GetShadowLightColor(a_BRDF, a_WorldPosition, N, V, u_FrameInfo.frameIndex);
    totalLightColor += GetIBLColor(a_BRDF, SampleBRDFLut(a_BRDF, NdotV), a_WorldPosition, N, V, NdotV);
    return totalLightColor;
}

void main()
{
    const vec4 textureSamplesMaterials[] = SampleTexturesMaterial(in_TexCoord);
    const BRDF brdf                      = GetBRDF(textureSamplesMaterials, in_Color);
    const vec3 emissive                  = GetEmissive(textureSamplesMaterials);
    vec4 color                           = vec4(0, 0, 0, 1);

    const vec4 fogScatteringTransmittance = FogGetScatteringTransmittance(u_Camera, in_WorldPosition);

#if MATERIAL_UNLIT
    color.rgb += brdf.cDiff;
    color.rgb += emissive;
    color.rgb = color.rgb * fogScatteringTransmittance.a + fogScatteringTransmittance.rgb;
    color.a   = brdf.transparency;
    return;
#else
    const float occlusion = GetOcclusion(textureSamplesMaterials);
    const vec3 normal     = GetNormal(textureSamplesMaterials, in_WorldTangent, in_WorldBitangent, in_WorldNormal);
    color.rgb += GetLightColor(brdf, in_WorldPosition, normal) * occlusion;
    color.rgb += emissive;
    color.rgb = color.rgb * fogScatteringTransmittance.a + fogScatteringTransmittance.rgb;
    color.a   = brdf.transparency;
#endif // MATERIAL_UNLIT

    if (color.a < u_Material.base.alphaCutoff)
        discard;
    out_Color    = color;
    vec3 a       = in_Position.xyz / in_Position.w * 0.5 + 0.5;
    vec3 b       = in_Position_Previous.xyz / in_Position_Previous.w * 0.5 + 0.5;
    out_Velocity = b.xy - a.xy;
}