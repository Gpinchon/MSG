#ifndef CAMERA_GLSL
#define CAMERA_GLSL

#include <Types.glsl>

#ifdef __cplusplus
namespace TabGraph::Renderer::GLSL {
#endif //__cplusplus

struct Camera {
    mat4x4 projection;
    mat4x4 view;
#ifdef __cplusplus
    bool operator!=(const Camera& a_Other)
    {
        return projection != a_Other.projection
            || view != a_Other.view;
    }
#endif //__cplusplus
};

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // CAMERA_GLSL
