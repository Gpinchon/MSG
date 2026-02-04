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
layout(location = 0) out vec3 out_VTInfo;
//////////////////////////////////////// STAGE OUTPUTS

//////////////////////////////////////// UNIFORMS
// None
//////////////////////////////////////// UNIFORMS

//////////////////////////////////////// SSBOS
layout(binding = 0) restrict readonly buffer VTMaterialBlock
{
    VTMaterialInfo ssbo_MaterialInfo;
};
layout(binding = 1) restrict readonly buffer VTSettingsBlock
{
    VTFeedbackSettings ssbo_FeedbackSettings;
};
//////////////////////////////////////// SSBOS

void main()
{
    const float transparency = GetTransparency(SampleCDiffMaterial(in_TexCoord));
    const float ditherVal    = Dither(ivec2(gl_FragCoord.xy));
    if (transparency < ditherVal)
        discard;
    VTTextureInfo texInfo = ssbo_MaterialInfo.textures[gl_Layer];
    if (texInfo.id == 0) { // no texture there
        out_VTInfo[0] = 0;
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
    vec2 finalUV  = wrappedTC / texInfo.texSize;
    float maxLod  = textureQueryLevels(u_MaterialSamplers[gl_Layer]);
    float lod     = VTComputeLOD(transformedTC, ssbo_FeedbackSettings.bufferRatio, ssbo_FeedbackSettings.maxAnisotropy);
    out_VTInfo[0] = uintBitsToFloat(texInfo.id);
    out_VTInfo[1] = uintBitsToFloat(packHalf2x16(finalUV));
    out_VTInfo[2] = lod; // / maxLod;
}