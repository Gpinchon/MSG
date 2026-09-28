#ifndef CAMERA_GLSL
#define CAMERA_GLSL

#include <Types.glsl>

#ifdef __cplusplus
namespace Msg::Renderer::GLSL {
#endif //__cplusplus

struct Camera {
    mat4x4 projection;
    mat4x4 view;
    float zNear;
    float zFar;
    vec2 jitter;
    vec3 position;
    uint _padding1[1];
#ifdef __cplusplus
    bool operator!=(const Camera& a_Other) const
    {
        return projection != a_Other.projection
            || view != a_Other.view
            || zNear != a_Other.zNear
            || zFar != a_Other.zFar
            || jitter != a_Other.jitter
            || position != a_Other.position;
    }
#endif //__cplusplus
};

struct CameraUBO {
    Camera current;
    Camera previous;
};

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // CAMERA_GLSL
