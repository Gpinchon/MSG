#ifndef FRAME_INFO_GLSL
#define FRAME_INFO_GLSL

#ifdef __cplusplus
#include <Types.glsl>
namespace Msg::Renderer::GLSL {
#endif //__cplusplus
struct FrameInfo {
    uint width;
    uint height;
    uint frameIndex;
    uint _padding[1];
};
#ifdef __cplusplus
}
#endif //__cplusplus

#endif // FRAME_INFO_GLSL