#include <Bindings.glsl>
#include <DeferredGBufferData.glsl>

layout(location = 0) in vec3 in_Cube_UV;
layout(location = 1) in vec4 in_Position;
layout(location = 2) in vec4 in_Position_Previous;

layout(binding = SAMPLERS_SKYBOX) uniform samplerCube u_Skybox;

//////////////////////////////////////// STAGE OUTPUTS
layout(location = OUTPUT_FRAG_DFD_GBUFFER0) out uvec4 out_GBuffer0;
layout(location = OUTPUT_FRAG_DFD_GBUFFER1) out uvec4 out_GBuffer1;
layout(location = OUTPUT_FRAG_DFD_VELOCITY) out vec2 out_Velocity;
layout(location = OUTPUT_FRAG_DFD_FINAL) out vec4 out_Final;
//////////////////////////////////////// STAGE OUTPUTS

void main()
{
    GBufferData gbufferData;
    out_Velocity = in_Position.xy - in_Position_Previous.xy;
    out_Final    = textureLod(u_Skybox, in_Cube_UV, 0);
}