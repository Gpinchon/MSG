//////////////////////////////////////// SHADER LAYOUT
layout(early_fragment_tests) in;
//////////////////////////////////////// SHADER LAYOUT

//////////////////////////////////////// INCLUDES
#include <BRDFInputs.glsl>
#include <Bindings.glsl>
#include <Camera.glsl>
#include <DeferredGBufferData.glsl>
#include <FogInputs.glsl>
#include <FrameInfo.glsl>
#include <Functions.glsl>
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
layout(location = 4 + ATTRIB_TEXCOORD_COUNT + 2) in vec4 in_Position;
layout(location = 4 + ATTRIB_TEXCOORD_COUNT + 3) in vec4 in_Position_Previous;
//////////////////////////////////////// STAGE INPUTS

//////////////////////////////////////// STAGE OUTPUTS
layout(location = 0) out vec4 out_Color;
//////////////////////////////////////// STAGE OUTPUTS

//////////////////////////////////////// UNIFORMS
layout(binding = IMG_OIT_VELOCITY, rg16f) writeonly uniform image3D img_Velocity;
layout(binding = IMG_OIT_GBUFFER0, rgba32ui) writeonly uniform uimage3D img_GBuffer0;
layout(binding = IMG_OIT_GBUFFER1, rgba32ui) writeonly uniform uimage3D img_GBuffer1;
layout(binding = IMG_OIT_DEPTH, r32ui) restrict readonly uniform uimage3D img_Depth;
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

uint UintDiff(uint a_X, uint a_Y)
{
    return a_X > a_Y ? (a_X - a_Y) : (a_Y - a_X);
}

uint FindClosestLayer(IN(uint) a_ZCur)
{
    // EARLY DEPTH TEST
    {
        uint zTest = imageLoad(img_Depth, ivec3(gl_FragCoord.xy, 0))[0];
        if (a_ZCur <= zTest)
            return 0;
    }
    {
        uint zTest = imageLoad(img_Depth, ivec3(gl_FragCoord.xy, OIT_LAYERS - 1))[0];
        if (a_ZCur >= zTest)
            return OIT_LAYERS - 1;
    }
    uint lo = 0;
    uint hi = OIT_LAYERS - 1;
    while (lo <= hi) {
        uint mid   = (hi + lo) / 2;
        uint zTest = imageLoad(img_Depth, ivec3(gl_FragCoord.xy, mid))[0];
        if (a_ZCur < zTest) {
            hi = mid - 1;
        } else if (a_ZCur > zTest) {
            lo = mid + 1;
        } else
            return mid;
    }
    uint zLo = imageLoad(img_Depth, ivec3(gl_FragCoord.xy, lo))[0];
    uint zHi = imageLoad(img_Depth, ivec3(gl_FragCoord.xy, hi))[0];
    return UintDiff(zLo, a_ZCur) < UintDiff(zHi, a_ZCur) ? lo : hi;
}

vec2 ComputeVelocity()
{
    vec3 a = in_Position.xyz / in_Position.w;
    vec3 b = in_Position_Previous.xyz / in_Position_Previous.w;
    a.xy += u_Camera.jitter;
    b.xy += u_Camera_Previous.jitter;
    a = a * 0.5 + 0.5;
    b = b * 0.5 + 0.5;
    return b.xy - a.xy;
}

vec3 GetLightColor(IN(BRDF) a_BRDF, IN(vec3) a_Normal)
{
    vec3 totalLightColor = vec3(0);
    const vec3 V         = normalize(u_Camera.position - in_WorldPosition);
    vec3 N               = gl_FrontFacing ? a_Normal : -a_Normal;
    float NdotV          = dot(N, V);
    VTFSSampleParameters params;
    params.brdf          = a_BRDF;
    params.brdfLutSample = SampleBRDFLut(a_BRDF, NdotV);
    params.worldPosition = in_WorldPosition;
    params.worldNormal   = N;
    params.worldView     = V;
    params.normalDotView = NdotV;
    params.NDCPosition   = in_NDCPosition;
    params.fragCoord     = gl_FragCoord.xy;
    params.frameIndex    = u_FrameInfo.frameIndex;
    totalLightColor += GetVTFSLightColor(params);
    return totalLightColor;
}

vec4 OITTaileBlend(
    IN(BRDF) a_BRDF,
    IN(vec4) a_TextureSamples[SAMPLERS_MATERIAL_COUNT])
{
    const vec3 emissive                   = GetEmissive(a_TextureSamples);
    vec4 color                            = vec4(0, 0, 0, 1);
    const vec4 fogScatteringTransmittance = FogGetScatteringTransmittance(u_Camera, in_WorldPosition);
    const float fogAlpha                  = 1 - fogScatteringTransmittance.a;
#if MATERIAL_UNLIT
    color.rgb += brdf.cDiff;
    color.rgb += emissive;
    color.rgb = color.rgb * (1 - fogAlpha) + fogScatteringTransmittance.rgb;
    color.a   = a_BRDF.transparency * (1 - fogAlpha) + fogAlpha;
#else
    const float occlusion = GetOcclusion(a_TextureSamples);
    const vec3 normal     = GetNormal(a_TextureSamples, in_WorldTangent, in_WorldBitangent, in_WorldNormal);
    color.rgb += GetLightColor(a_BRDF, normal) * occlusion;
    color.rgb += emissive;
    color.rgb = color.rgb * (1 - fogAlpha) + fogScatteringTransmittance.rgb;
    color.a   = a_BRDF.transparency * (1 - fogAlpha) + fogAlpha;
#endif // MATERIAL_UNLIT
    return color;
}

vec4 OITWriteLayer(
    IN(BRDF) a_BRDF,
    IN(vec4) a_TextureSamples[SAMPLERS_MATERIAL_COUNT],
    IN(uint) a_Layer)
{
#if MATERIAL_UNLIT
    float occlusion = 1;
    vec3 normal     = in_WorldNormal;
    normal          = gl_FrontFacing ? normal : -normal;
#else
    float occlusion = GetOcclusion(a_TextureSamples);
    vec3 normal     = GetNormal(a_TextureSamples, in_WorldTangent, in_WorldBitangent, in_WorldNormal);
    normal          = gl_FrontFacing ? normal : -normal;
#endif
    GBufferData gBufferData;
    gBufferData.brdf                    = a_BRDF;
    gBufferData.emissive                = GetEmissive(a_TextureSamples);
    gBufferData.AO                      = occlusion;
    gBufferData.shadingModelID          = MATERIAL_TYPE;
    gBufferData.unlit                   = MATERIAL_UNLIT == 1;
    gBufferData.normal                  = normal;
    gBufferData.ndcDepth                = in_NDCPosition.z;
    GBufferDataPacked gBufferDataPacked = PackGBufferData(gBufferData);
    imageStore(img_Velocity, ivec3(gl_FragCoord.xy, a_Layer), vec4(ComputeVelocity(), 0, 0));
    imageStore(img_GBuffer0, ivec3(gl_FragCoord.xy, a_Layer), gBufferDataPacked.data0);
    imageStore(img_GBuffer1, ivec3(gl_FragCoord.xy, a_Layer), gBufferDataPacked.data1);
    return vec4(0); // fragment was written, write nothing to tail blend
}

vec4 OITWritePixel(
    IN(BRDF) a_BRDF,
    IN(vec4) a_TextureSamples[SAMPLERS_MATERIAL_COUNT])
{
    const uint zCur = floatBitsToUint(gl_FragCoord.z);
    // EARLY DEPTH TEST
    {
        // if we're behind the farthest fragment, tail blend
        const uint zTest = imageLoad(img_Depth, ivec3(gl_FragCoord.xy, OIT_LAYERS - 1))[0];
        if (zTest < zCur)
            return OITTaileBlend(a_BRDF, a_TextureSamples);
    }
    // find this fragment's index through binary search
    // search for closest value because of floating point precision
    return OITWriteLayer(a_BRDF, a_TextureSamples, FindClosestLayer(zCur));
}

void main()
{
    const vec4 textureSamplesMaterials[] = SampleTexturesMaterial(in_TexCoord);
    const BRDF brdf                      = GetBRDF(textureSamplesMaterials, in_Color);
    float ditherVal                      = normalizeValue(clamp(in_NDCPosition.z * 0.5 + 0.5, 0, 0.025f), 0, 0.025f);
    float randVal                        = Dither(ivec2(gl_FragCoord.xy));
    if (brdf.transparency <= 0.003 || randVal >= ditherVal)
        discard;
    out_Color = OITWritePixel(brdf, textureSamplesMaterials);
    out_Color.rgb *= out_Color.a;
}