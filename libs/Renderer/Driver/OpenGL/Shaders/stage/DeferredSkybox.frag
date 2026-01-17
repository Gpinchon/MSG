#include <Bindings.glsl>

layout(location = 0) in vec3 in_Cube_UV;
layout(location = 1) in vec4 in_Position;
layout(location = 2) in vec4 in_Position_Previous;

layout(binding = SAMPLERS_SKYBOX) uniform samplerCube u_Skybox;

//////////////////////////////////////// STAGE OUTPUTS
layout(location = OUTPUT_FRAG_VELOCITY) out vec2 out_Velocity;
layout(location = OUTPUT_FRAG_FINAL) out vec4 out_Final;
//////////////////////////////////////// STAGE OUTPUTS

void main()
{
    out_Velocity = in_Position.xy - in_Position_Previous.xy;
    out_Final    = textureLod(u_Skybox, in_Cube_UV, 0);
}