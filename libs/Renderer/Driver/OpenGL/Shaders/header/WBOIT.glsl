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
#define WBOIT_LAYERS_DEPTH 1.f
#ifndef __cplusplus
layout(binding = IMG_WBOIT_ACCUM, rgba16f) coherent uniform image3D img_accum;
layout(binding = IMG_WBOIT_REV, r8) coherent uniform image3D img_revealage;
layout(binding = IMG_WBOIT_DEPTH, r32f) readonly uniform image2D img_Depth;

float WBOITGetLayerIndex(IN(float) a_ViewDist)
{
    float backViewDist = imageLoad(img_Depth, ivec2(gl_FragCoord.xy))[0];
    float dist         = max(0.f, a_ViewDist - backViewDist);
    return min(dist / WBOIT_LAYERS_DEPTH, imageSize(img_Depth).z - 1);
}

void WBOITWritePixel(IN(vec4) a_Color, IN(vec3) a_Transmition, IN(float) a_ViewDist)
{
    float layerIndex  = WBOITGetLayerIndex(a_ViewDist);
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