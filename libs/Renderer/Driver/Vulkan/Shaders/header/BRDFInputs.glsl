#ifndef BRDF_INPUTS_GLSL
#define BRDF_INPUTS_GLSL
#ifndef __cplusplus
#include <BRDF.glsl>
#include <Bindings.glsl>

layout(binding = SAMPLERS_BRDF_LUT) uniform sampler2D u_BRDFLut;

vec2 SampleBRDFLut(IN(BRDF) a_BRDF, IN(float) a_Theta)
{
    return texture(u_BRDFLut, vec2(a_Theta, a_BRDF.alpha)).xy;
}
#endif //__cplusplus
#endif // BRDF_INPUTS_GLSL