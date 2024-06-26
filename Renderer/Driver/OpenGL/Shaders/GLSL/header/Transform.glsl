#ifndef TRANSFORM_GLSL
#define TRANSFORM_GLSL

#include <Types.glsl>

#ifdef __cplusplus
namespace TabGraph::Renderer::GLSL {
#endif //__cplusplus

struct Transform {
    mat4x4 matrix;
};

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // TRANSFORM_GLSL