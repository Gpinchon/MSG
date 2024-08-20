#include <Bindings.glsl>

layout(location = 0) in vec3 in_Cube_UV;

layout(binding = SAMPLERS_SKYBOX) uniform samplerCube u_Skybox;

layout(location = OUTPUT_FRAG_FWD_OPAQUE_COLOR) out vec3 out_Color;

void main()
{
    out_Color = textureLod(u_Skybox, in_Cube_UV, 0).rgb;
}