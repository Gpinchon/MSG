#include <Bindings.glsl>

layout(location = 0) in vec3 in_Cube_UV;
layout(location = 1) in vec4 in_Position;
layout(location = 2) in vec4 in_Position_Previous;

layout(binding = SAMPLERS_SKYBOX) uniform samplerCube u_Skybox;

layout(location = OUTPUT_FRAG_FWD_OPAQUE_COLOR) out vec4 out_Color;
layout(location = OUTPUT_FRAG_FWD_OPAQUE_VELOCITY) out vec2 out_Velocity;

void main()
{
    out_Color    = textureLod(u_Skybox, in_Cube_UV, 0);
    out_Velocity = in_Position.xy - in_Position_Previous.xy;
}