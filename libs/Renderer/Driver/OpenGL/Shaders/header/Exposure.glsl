#ifndef EXPOSURE_GLSL
#define EXPOSURE_GLSL

#ifdef __cplusplus
#include <Types.glsl>
namespace Msg::Renderer::GLSL {
#endif //__cplusplus
struct AutoExposureSettings {
    float minLogLum;
    float maxLogLum;
    float key;
    float deltaTime;
};
#ifdef __cplusplus
}
#endif //__cplusplus

#endif // EXPOSURE_GLSL