#ifndef FOG_AREA_GLSL
#define FOG_AREA_GLSL

#include <SDF.glsl>

#define FOG_AREA_OP_ADD 0
#define FOG_AREA_OP_REP 1

#ifdef __cplusplus
namespace MSG::Renderer::GLSL {
#endif //__cplusplus
struct FogArea {
    ShapeComb shapeComb;
    vec3 scattering;
    float extinction;
    vec3 emissive;
    float phaseG;
    float attenuationExp;
    uint op;
    uint _padding[2];
};
#ifdef __cplusplus
}
#endif //__cplusplus
#endif // FOG_AREA_GLSL