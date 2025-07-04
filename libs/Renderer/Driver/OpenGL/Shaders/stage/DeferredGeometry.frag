//////////////////////////////////////// INCLUDES
#include <BRDF.glsl>
#include <Bindings.glsl>
#include <Camera.glsl>
#include <DeferredGBufferData.glsl>
#include <FrameInfo.glsl>
#include <Functions.glsl>
#include <MaterialInputs.glsl>
#include <Random.glsl>
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
layout(location = OUTPUT_FRAG_DFD_GBUFFER0) out uvec4 out_GBuffer0;
layout(location = OUTPUT_FRAG_DFD_GBUFFER1) out uvec4 out_GBuffer1;
layout(location = OUTPUT_FRAG_DFD_VELOCITY) out vec2 out_Velocity;
layout(location = OUTPUT_FRAG_DFD_FINAL) out vec4 out_Final;
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

void main()
{
    GBufferData outData;
    const vec4 textureSamplesMaterials[] = SampleTexturesMaterial(in_TexCoord);
    outData.brdf                         = GetBRDF(textureSamplesMaterials, in_Color);
    outData.shadingModelID               = MATERIAL_TYPE;
    outData.ndcDepth                     = in_NDCPosition.z;

#if MATERIAL_UNLIT
    outData.unlit    = true;
    outData.emissive = vec3(0);
    outData.AO       = 0;
    outData.normal   = in_WorldNormal;
#else
    outData.unlit    = false;
    outData.emissive = GetEmissive(textureSamplesMaterials);
    outData.AO       = GetOcclusion(textureSamplesMaterials);
    outData.normal   = GetNormal(textureSamplesMaterials, in_WorldTangent, in_WorldBitangent, in_WorldNormal);
#endif // MATERIAL_UNLIT
    outData.normal = gl_FrontFacing ? outData.normal : -outData.normal;

    float ditherVal = normalizeValue(clamp(in_NDCPosition.z * 0.5 + 0.5, 0, 0.025f), 0, 0.025f);
    float randVal   = Dither(ivec2(gl_FragCoord.xy));
    if (outData.brdf.transparency < u_Material.base.alphaCutoff || randVal >= ditherVal)
        discard;

    GBufferDataPacked packedData = PackGBufferData(outData);
    vec3 a                       = in_Position.xyz / in_Position.w;
    vec3 b                       = in_Position_Previous.xyz / in_Position_Previous.w;
    a.xy += u_Camera.jitter;
    b.xy += u_Camera_Previous.jitter;
    a            = a * 0.5 + 0.5;
    b            = b * 0.5 + 0.5;
    out_GBuffer0 = packedData.data0;
    out_GBuffer1 = packedData.data1;
    out_Velocity = b.xy - a.xy;
}