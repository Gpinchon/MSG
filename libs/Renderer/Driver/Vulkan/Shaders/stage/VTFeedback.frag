#if MATERIAL_ALPHA_MODE == MATERIAL_ALPHA_MODE_OPAQUE
layout(early_fragment_tests) in;
#endif
//////////////////////////////////////// INCLUDES
#include <Bindings.glsl>
#include <Functions.glsl>
#include <MaterialInputs.glsl>
#include <Random.glsl>
//////////////////////////////////////// INCLUDES

//////////////////////////////////////// STAGE INPUTS
layout(location = 0) in vec2 in_TexCoord[ATTRIB_TEXCOORD_COUNT];
//////////////////////////////////////// STAGE INPUTS

//////////////////////////////////////// STAGE OUTPUTS
layout(location = 0) out uvec3 out_VTInfo;
//////////////////////////////////////// STAGE OUTPUTS

//////////////////////////////////////// UNIFORMS
// None
//////////////////////////////////////// UNIFORMS

//////////////////////////////////////// SSBOS
layout(binding = 0) restrict readonly buffer VTMaterialBlock
{
    VTFeedbackMaterialInfo ssbo_MaterialInfo;
};
layout(binding = UBO_VT_FEEDBACK_SETTINGS) uniform VTFedbackSettingsBlock
{
    VTFeedbackSettings ubo_FeedbackSettings;
};
//////////////////////////////////////// SSBOS

void main()
{
#if MATERIAL_ALPHA_MODE != MATERIAL_ALPHA_MODE_OPAQUE
    const float transparency = GetTransparency(SampleCDiffMaterial(in_TexCoord));
    const float ditherVal    = Dither(ivec2(gl_FragCoord.xy));
    if (transparency < ditherVal)
        discard;
#endif
    const VTFeedbackInfo feedbackTexInfo = ssbo_MaterialInfo.textures[gl_Layer];
    const VTInfo texInfo                 = feedbackTexInfo.info;
    if (feedbackTexInfo.id == uvec2(0)) { // no texture there
        out_VTInfo.xy = uvec2(0);
        return;
    }
    const vec2 uv        = in_TexCoord[texInfo.texCoord].xy;
    const vec2 scale     = texInfo.transform.scale;
    const vec2 offset    = texInfo.transform.offset;
    const float rotation = texInfo.transform.rotation;
    mat3 rotationMat     = mat3(
        cos(rotation), sin(rotation), 0,
        -sin(rotation), cos(rotation), 0,
        0, 0, 1);
    vec2 transformedUV = (rotationMat * vec3(uv, 1)).xy * scale + offset;
    vec2 transformedTC = transformedUV * texInfo.texSize;
    vec2 wrappedTC     = WrapTexelCoords(
        texInfo.wrapS, texInfo.wrapT,
        texInfo.texSize,
        transformedTC);
    vec2 wrappedUV = wrappedTC / texInfo.texSize;
    float maxLod   = float(texInfo.levels);
    float lod      = VTComputeLOD(wrappedTC, ubo_FeedbackSettings.bufferRatio, u_Settings.maxAniso);
    lod            = clamp(lod + u_Settings.lodBias, 0, maxLod) / maxLod;
    out_VTInfo.xy  = feedbackTexInfo.id;
    out_VTInfo.z   = packUnorm4x8(vec4(wrappedUV, 0, lod));
}