#ifndef SSAO_GLSL
#define SSAO_GLSL
#ifdef __cplusplus
#include <Types.glsl>

namespace MSG::Renderer::GLSL {
#endif //__cplusplus
struct SSAOSettings {
    float radius;
    float strength;
    uint _padding[2];
};
#ifdef __cplusplus
static_assert(sizeof(SSAOSettings) % 16 == 0);
}
#endif //__cplusplus
#endif // SSAO_GLSL