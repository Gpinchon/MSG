#ifndef WBOIT_GLSL
#define WBOIT_GLSL
#ifndef __cplusplus
#include <Bindings.glsl>
#include <Camera.glsl>
#include <Functions.glsl>
#endif //__cplusplus
#ifdef __cplusplus
#include <Types.glsl>
namespace MSG::Renderer::GLSL {
#endif //__cplusplus
#define WBOIT_LAYERS       11
#define WBOIT_LAYERS_DEPTH 2.f
#ifndef __cplusplus
layout(binding = SAMPLERS_WBOIT_DEPTH) uniform sampler2D u_WBOITDepth;
layout(binding = IMG_WBOIT_ACCUM, rgba16f) coherent uniform image3D img_accum;
layout(binding = IMG_WBOIT_REV, r8) coherent uniform image3D img_revealage;

float WBOITGetLayerIndex(IN(vec3) a_NDCPos, IN(Camera) a_Camera)
{
    vec2 uv            = gl_FragCoord.xy / textureSize(u_WBOITDepth, 0).xy;
    float backNDCDepth = texture(u_WBOITDepth, uv)[0];
    vec3 backNDCPos    = vec3(uv, backNDCDepth) * 2.f - 1.f;
    vec4 backViewPos   = inverse(a_Camera.projection) * vec4(backNDCPos, 1);
    float backViewDist = -(backViewPos.z / backViewPos.w);

    vec4 viewPos   = inverse(a_Camera.projection) * vec4(a_NDCPos, 1);
    float viewDist = -(viewPos.z / viewPos.w);

    float dist = max(0.f, viewDist - backViewDist);
    return min(dist / WBOIT_LAYERS_DEPTH, WBOIT_LAYERS - 1);
}

void WBOITWritePixel(IN(vec4) a_Color, IN(vec3) a_Transmition, IN(vec3) a_NDCPos, IN(Camera) a_Camera)
{
    float layerIndex  = WBOITGetLayerIndex(a_NDCPos, a_Camera);
    float layerDepth  = fract(layerIndex);
    ivec3 outputCoord = ivec3(gl_FragCoord.xy, layerIndex);

    vec4 premultipliedReflect = vec4(a_Color.rgb * a_Color.a, a_Color.a);
    // premultipliedReflect.a *= 1.0 - (a_Transmition.r + a_Transmition.g + a_Transmition.b) / 3.0;
    float tmp = (premultipliedReflect.a * 8.0 + 0.01) * (-layerDepth * 0.95 + 1.0);
    float w   = clamp(tmp * tmp * tmp * 1e3, 1e-2, 3e2);

    imageStore(img_accum, outputCoord, imageLoad(img_accum, outputCoord) + premultipliedReflect * w);
    imageStore(img_revealage, outputCoord, vec4(imageLoad(img_revealage, outputCoord).r * (1 - premultipliedReflect.a)));
}
#endif //__cplusplus
#ifdef __cplusplus
}
#endif //__cplusplus
#endif // WBOIT_GLSL