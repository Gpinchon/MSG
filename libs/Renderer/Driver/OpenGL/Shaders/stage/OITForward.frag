//////////////////////////////////////// SHADER LAYOUT
layout(early_fragment_tests) in;
//////////////////////////////////////// SHADER LAYOUT

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
#include <OIT.glsl>
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
layout(location = 0) out vec4 out_Color;
//////////////////////////////////////// STAGE OUTPUTS

//////////////////////////////////////// UNIFORMS
layout(binding = IMG_OIT_COLORS, rgba16f) writeonly uniform image3D img_Colors;
layout(binding = IMG_OIT_DEPTH, r32f) restrict readonly uniform image3D img_Depth;
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

vec4 OITWritePixel(IN(vec4) a_Color)
{
    //EARLY DEPTH TEST
    {
        //if we're behind the farthest fragment, tail blend
        const float zTest = imageLoad(img_Depth, ivec3(gl_FragCoord.xy, OIT_LAYERS - 1))[0];
        if (zTest < gl_FragCoord.z)
            return a_Color;
    }
    // find this fragment's index through binary search
    int start = 0;
    int end  =  OIT_LAYERS - 1;
    while (start < end) {
        int mid = (start + end) / 2;
        const float zTest = imageLoad(img_Depth, ivec3(gl_FragCoord.xy, mid))[0];
        if (zTest < gl_FragCoord.z)
            start = mid + 1;
        else
            end = mid;
    }
    imageStore(img_Colors, ivec3(gl_FragCoord.xy, start), a_Color);
    return vec4(0);
}

vec3 GetLightColor(IN(BRDF) a_BRDF, IN(vec3) a_WorldPosition, IN(vec3) a_Normal, IN(float) a_FogTransmittance)
{
    vec3 totalLightColor = vec3(0);
    const vec3 V         = normalize(u_Camera.position - a_WorldPosition);
    vec3 N               = gl_FrontFacing ? a_Normal : -a_Normal;
    float NdotV          = dot(N, V);
    totalLightColor += GetVTFSLightColor(a_BRDF, a_WorldPosition, in_NDCPosition, N, V);
    totalLightColor += GetShadowLightColor(a_BRDF, a_WorldPosition, (1 - a_FogTransmittance), N, V, u_FrameInfo.frameIndex);
    totalLightColor += GetIBLColor(a_BRDF, SampleBRDFLut(a_BRDF, NdotV), a_WorldPosition, N, V, NdotV);
    return totalLightColor;
}

void main()
{
    const vec4 textureSamplesMaterials[]  = SampleTexturesMaterial(in_TexCoord);
    const BRDF brdf                       = GetBRDF(textureSamplesMaterials, in_Color);
    const vec3 emissive                   = GetEmissive(textureSamplesMaterials);
    vec4 color                            = vec4(0, 0, 0, 1);
    const vec4 fogScatteringTransmittance = FogGetScatteringTransmittance(u_Camera, in_WorldPosition);
#if MATERIAL_UNLIT
    color.rgb += brdf.cDiff;
    color.rgb += emissive;
    color.rgb = color.rgb * fogScatteringTransmittance.a + fogScatteringTransmittance.rgb;
    color.a   = brdf.transparency;
#else
    const float occlusion = GetOcclusion(textureSamplesMaterials);
    const vec3 normal     = GetNormal(textureSamplesMaterials, in_WorldTangent, in_WorldBitangent, in_WorldNormal);
    color.rgb += GetLightColor(brdf, in_WorldPosition, normal, fogScatteringTransmittance.a) * occlusion;
    color.rgb += emissive;
    color.rgb = color.rgb * fogScatteringTransmittance.a + fogScatteringTransmittance.rgb;
    color.a   = brdf.transparency;
#endif // MATERIAL_UNLIT
    if (brdf.transparency <= 0.003)
        discard;
    out_Color = OITWritePixel(color);
    out_Color.rgb *= out_Color.a;
}