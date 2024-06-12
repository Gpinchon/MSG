#ifndef GLSL_CAMERA_UBO
#define GLSL_CAMERA_UBO

#include <Types.glsl>

#ifdef __cplusplus
namespace TabGraph::Renderer::GLSL {
#endif //__cplusplus

struct CameraUBO {
    mat4x4 projection;
    mat4x4 view;
    bool operator!=(const CameraUBO& a_Other)
    {
        return projection != a_Other.projection
            || view != a_Other.view;
    }
};

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // GLSL_CAMERA_UBO
