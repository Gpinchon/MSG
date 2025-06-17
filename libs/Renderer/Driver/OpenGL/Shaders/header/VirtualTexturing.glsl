#ifndef VIRTUAL_TEXTURING_GLSL
#define VIRTUAL_TEXTURING_GLSL

#include <Bindings.glsl>
#include <Material.glsl>

#define VT_WRAP_CLAMP         0
#define VT_WRAP_CLAMP_MIRROR  1
#define VT_WRAP_REPEAT        2
#define VT_WRAP_REPEAT_MIRROR 3

#ifdef __cplusplus
#include <Types.glsl>
namespace MSG::Renderer::GLSL {
#endif
struct VTFeedbackOutput {
    vec2 minUV;
    vec2 maxUV;
    float minMip;
    float maxMip;
    uint _padding[2];
};

struct VTFeedbackOutputInternal {
    uint minU;
    uint minV;
    uint maxU;
    uint maxV;
    uint minMip;
    uint maxMip;
    uint _padding[2];
};

struct VTTextureInfo {
    TextureTransform transform;
    uint texCoord;
    uint id;
    uint wrapS;
    uint wrapT;
    float maxAniso;
    uint _padding[1];
    vec2 texSize;
};

struct VTMaterialInfo {
    VTTextureInfo textures[SAMPLERS_MATERIAL_COUNT];
};
#ifdef __cplusplus
static_assert(sizeof(VTFeedbackOutput) % 16 == 0);
static_assert(sizeof(VTTextureInfo) % 16 == 0);
static_assert(sizeof(VTMaterialInfo) % 16 == 0);
}
#endif
#endif // VIRTUAL_TEXTURING_GLSL