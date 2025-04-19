#ifndef DEFERRED_G_BUFFER_DATA_GLSL
#define DEFERRED_G_BUFFER_DATA_GLSL
#ifndef __cplusplus
#include <BRDF.glsl>
#include <Types.glsl>

struct GBufferData {
    BRDF brdf;
    vec3 emissive;
    float AO;
    uint shadingModelID;
    bool unlit;
    vec3 normal;
    float ndcDepth;
};

struct GBufferDataPacked {
    uvec4 data0;
    uvec4 data1;
};

GBufferDataPacked PackGBufferData(IN(GBufferData) a_Data)
{
    GBufferDataPacked packedData;
    packedData.data0[0] = packUnorm4x8(vec4(a_Data.brdf.cDiff, a_Data.brdf.alpha));
    packedData.data0[1] = packUnorm4x8(vec4(a_Data.brdf.f0, a_Data.brdf.transparency));
    packedData.data0[2] = packUnorm4x8(vec4(a_Data.emissive, a_Data.AO));
    packedData.data0[3] = packUnorm2x16(vec2(a_Data.shadingModelID, a_Data.unlit));
    packedData.data1[0] = packSnorm4x8(vec4(a_Data.normal, 0));
    packedData.data1[1] = floatBitsToUint(a_Data.ndcDepth);
    return packedData;
}

GBufferData UnpackGBufferData(IN(GBufferDataPacked) a_Data)
{
    vec4 cDiffAlpha     = unpackUnorm4x8(a_Data.data0[0]);
    vec4 f0Transp       = unpackUnorm4x8(a_Data.data0[1]);
    vec4 emissiveAO     = unpackUnorm4x8(a_Data.data0[2]);
    vec2 shadingIDUnlit = unpackUnorm2x16(a_Data.data0[3]);
    vec4 normal         = unpackSnorm4x8(a_Data.data1[0]);
    float depth         = uintBitsToFloat(a_Data.data1[1]);
    GBufferData data;
    data.brdf.cDiff        = cDiffAlpha.rgb;
    data.brdf.alpha        = cDiffAlpha.a;
    data.brdf.f0           = f0Transp.rgb;
    data.brdf.transparency = f0Transp.a;
    data.emissive          = emissiveAO.rgb;
    data.AO                = emissiveAO.a;
    data.shadingModelID    = uint(shadingIDUnlit.r);
    data.unlit             = bool(shadingIDUnlit.g);
    data.normal            = normal.rgb;
    data.ndcDepth          = depth;
    return data;
}
#endif //__cplusplus
#endif // DEFERRED_G_BUFFER_DATA_GLSL