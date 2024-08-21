#ifndef TRANSFORM_GLSL
#define TRANSFORM_GLSL

#include <Types.glsl>

#ifdef __cplusplus
namespace TabGraph::Renderer::GLSL {
#endif //__cplusplus

struct Transform {
    mat4x4 modelMatrix;  // to transform model position to world
    mat4x4 normalMatrix; // to transform model normal to world
};

struct TransformUBO {
    Transform current;  // used to project mesh
    Transform previous; // used to compute velocity
};

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // TRANSFORM_GLSL