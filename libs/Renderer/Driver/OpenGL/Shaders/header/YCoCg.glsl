#ifndef YCOCG_GLSL
#define YCOCG_GLSL

#ifdef __cplusplus
#include <Types.glsl>
namespace Msg::Renderer::GLSL {
#endif
vec3 RGB2YCoCg(vec3 a_RGB)
{
    const mat3 convMat = mat3(
        1 / 4.f, 1 / 2.f, 1 / 4.f,
        1 / 2.f, 0.f, -1 / 2.f,
        -1 / 4.f, 1 / 2.f, -1 / 4.f);
    return convMat * a_RGB;
}
vec4 RGBA2YCoCgA(vec4 a_RGBA)
{
    return vec4(RGB2YCoCg(a_RGBA.rgb), a_RGBA.a);
}

vec3 YCoCg2RGB(vec3 a_YCoCg)
{
    const mat3 convMat = mat3(
        1, 1, -1,
        1, 0, 1,
        1, -1, -1);
    return convMat * a_YCoCg;
}
vec4 YCoCgA2RGBA(vec4 a_YCoCgA)
{
    return vec4(YCoCg2RGB(a_YCoCgA.rgb), a_YCoCgA.a);
}
#endif __cplusplus

#endif // YCOCG_GLSL