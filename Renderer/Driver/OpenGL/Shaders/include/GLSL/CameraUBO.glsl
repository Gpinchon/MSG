#ifndef GLSL_CAMERA_UBO
#define GLSL_CAMERA_UBO

#ifdef __cplusplus
#include <GLSL/Types.glsl>
namespace TabGraph::Renderer::GLSL {
#else //__cplusplus
#include <Types.glsl>
#endif //__cplusplus

struct CameraUBO {
    MAT4x4 projection;
    MAT4x4 view;
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
