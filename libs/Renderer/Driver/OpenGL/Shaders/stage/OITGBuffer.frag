#include <BRDFInputs.glsl>
#include <Bindings.glsl>
#include <Camera.glsl>
#include <DeferredGBufferData.glsl>
#include <FrameInfo.glsl>
#include <Functions.glsl>
#include <OIT.glsl>
#include <Random.glsl>

layout(binding = UBO_FRAME_INFO) uniform FrameInfoBlock
{
    FrameInfo u_FrameInfo;
};
layout(binding = UBO_CAMERA) uniform CameraBlock
{
    Camera u_Camera;
};

layout(binding = IMG_OIT_VELOCITY, rg16f) restrict readonly uniform image3D img_Velocity;
layout(binding = IMG_OIT_GBUFFER0, rgba32ui) restrict readonly uniform uimage3D img_GBuffer0;
layout(binding = IMG_OIT_GBUFFER1, rgba32ui) restrict readonly uniform uimage3D img_GBuffer1;
layout(binding = IMG_OIT_DEPTH, r32ui) restrict readonly uniform uimage3D img_Depth;

layout(binding = IMG_OIT_OPAQUE_VELOCITY, rg16f) restrict writeonly uniform image2D img_BackVelocity;
layout(binding = IMG_OIT_OPAQUE_GBUFFER0, rgba32ui) restrict writeonly uniform uimage2D img_BackGBuffer0;
layout(binding = IMG_OIT_OPAQUE_GBUFFER1, rgba32ui) restrict writeonly uniform uimage2D img_BackGBuffer1;

layout(location = 0) in invariant vec2 in_UV;

float GetTransparency(IN(vec2) a_Velocity, IN(uvec4) a_GData0, IN(uvec4) a_GData1)
{
    GBufferDataPacked gbufferDataPacked;
    gbufferDataPacked.data0 = a_GData0;
    gbufferDataPacked.data1 = a_GData1;
    GBufferData gBufferData = UnpackGBufferData(gbufferDataPacked);
    return gBufferData.brdf.transparency;
}

void main()
{
    float totalTransparency = 0;
    bool gDataWritten       = false;
    for (uint layer = 0; layer < OIT_LAYERS && !gDataWritten && totalTransparency < 1; layer++) {
        float ditherVal = Dither(ivec2(gl_FragCoord.xy + layer));
        ivec3 texCoord  = ivec3(gl_FragCoord.xy, layer);
        if (imageLoad(img_Depth, texCoord)[0] == 0xFFFFFFFFu)
            break; // no more occupied layers
        vec4 velocity      = imageLoad(img_Velocity, texCoord);
        uvec4 gBuffer0     = imageLoad(img_GBuffer0, texCoord);
        uvec4 gBuffer1     = imageLoad(img_GBuffer1, texCoord);
        float transparency = GetTransparency(velocity.xy, gBuffer0, gBuffer1);
        if (transparency > ditherVal) {
            imageStore(img_BackVelocity, texCoord.xy, velocity);
            imageStore(img_BackGBuffer0, texCoord.xy, gBuffer0);
            imageStore(img_BackGBuffer1, texCoord.xy, gBuffer1);
            gDataWritten = true;
        }
        totalTransparency += (1 - totalTransparency) * transparency;
    }
    if (!gDataWritten)
        discard; // don't write in stencil buffer
}