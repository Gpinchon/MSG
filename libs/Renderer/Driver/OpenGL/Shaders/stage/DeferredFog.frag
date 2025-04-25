#include <Bindings.glsl>
#include <Camera.glsl>
#include <DeferredGBufferData.glsl>
#include <FogInputs.glsl>

//////////////////////////////////////// STAGE INPUTS
layout(location = 0) in vec2 in_UV;
//////////////////////////////////////// STAGE INPUTS

//////////////////////////////////////// STAGE OUTPUTS
layout(location = 0) out vec4 out_Final;
//////////////////////////////////////// STAGE OUTPUTS

//////////////////////////////////////// UNIFORMS
layout(binding = UBO_CAMERA) uniform CameraBlock
{
    Camera u_Camera;
};
layout(binding = 0, rgba32ui) restrict uniform uimage2D img_GBuffer0;
layout(binding = 1, rgba32ui) restrict uniform uimage2D img_GBuffer1;
//////////////////////////////////////// UNIFORMS

void main()
{
    ivec2 texCoord = ivec2(gl_FragCoord.xy);
    GBufferDataPacked gbufferDataPacked;
    gbufferDataPacked.data0 = imageLoad(img_GBuffer0, texCoord);
    gbufferDataPacked.data1 = imageLoad(img_GBuffer1, texCoord);
    GBufferData gBufferData = UnpackGBufferData(gbufferDataPacked);

    if (gBufferData.ndcDepth == 0) {
        if (!u_FogSettings.fogBackground) {
            out_Final = vec4(0, 0, 0, 1);
            return;
        }
        gBufferData.ndcDepth = 0.99;
    }

    const mat4x4 VP     = u_Camera.projection * u_Camera.view;
    const mat4x4 invVP  = inverse(VP);
    const vec3 NDCPos   = vec3(in_UV * 2.f - 1.f, gBufferData.ndcDepth);
    const vec4 projPos  = invVP * vec4(NDCPos, 1);
    const vec3 worldPos = projPos.xyz / projPos.w;

    out_Final = FogGetScatteringTransmittance(u_Camera, worldPos);
}