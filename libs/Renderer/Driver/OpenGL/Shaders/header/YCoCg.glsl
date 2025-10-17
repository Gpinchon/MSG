#ifndef YCOCG_GLSL
#define YCOCG_GLSL

#ifdef __cplusplus
#include <Types.glsl>
namespace Msg::Renderer::GLSL {
#endif
vec4 RGBA2YCoCgA(vec4 inRGBA)
{
    const mat3 convMat = mat3(
        1 / 4.f, 1 / 2.f, 1 / 4.f,
        1 / 2.f, 0.f, -1 / 2.f,
        -1 / 4.f, 1 / 2.f, -1 / 4.f);
    return vec4(convMat * inRGBA.rgb, inRGBA.a);
}

vec4 YCoCgA2RGBA(vec4 inYCoCgA)
{
    const mat3 convMat = mat3(
        1, 1, -1,
        1, 0, 1,
        1, -1, -1);
    return vec4(convMat * inYCoCgA.rgb, inYCoCgA.a);
}
#endif __cplusplus

#endif // YCOCG_GLSL