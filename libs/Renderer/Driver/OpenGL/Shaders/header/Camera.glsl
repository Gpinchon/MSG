#ifndef CAMERA_GLSL
#define CAMERA_GLSL

#include <Types.glsl>

#ifdef __cplusplus
namespace MSG::Renderer::GLSL {
#endif //__cplusplus

struct Camera {
    mat4x4 projection;
    mat4x4 view;
    float zNear;
    float zFar;
    uint _padding0[2];
    vec3 position;
    uint _padding1[1];
#ifdef __cplusplus
    bool operator!=(const Camera& a_Other)
    {
        return position != a_Other.position
            || projection != a_Other.projection
            || view != a_Other.view;
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
