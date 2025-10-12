#ifndef FOG_CAMERA_GLSL
#define FOG_CAMERA_GLSL
#include <Camera.glsl>

#ifdef __cplusplus
#include <Types.glsl>
namespace Msg::Renderer::GLSL {
#endif //__cplusplus
struct FogCamera {
    Camera current;
    Camera previous;
    uint _padding[48];
};
#ifdef __cplusplus
static_assert(sizeof(FogCamera) % 16 == 0);
static_assert(sizeof(FogCamera) % 256 == 0);
}
#endif //__cplusplus
#endif // FOG_CAMERA_GLSL