#ifndef PPLL_GLSL
#define PPLL_GLSL
#define PPLL_SIZE
#ifdef __cplusplus
#include <Types.glsl>
namespace MSG::Renderer::GLSL {
#endif //__cplusplus
struct LinkedListEntry {
    vec4 color;
    float depth;
    uint next;
    uint _padding[2];
};
#ifdef __cplusplus
static_assert(sizeof(LinkedListEntry) % 16 == 0);
}
#endif //__cplusplus
#endif // PPLL_GLSL