#include <Bindings.glsl>
#include <Camera.glsl>

layout(binding = UBO_CAMERA) uniform CameraBlock
{
    Camera u_Camera;
};

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) out vec3 out_Cube_UV;

void main()
{
    const mat4x4 invProj = inverse(u_Camera.projection);
    const mat4x4 invView = inverse(u_Camera.view);
    const float x        = -1.0 + float((gl_VertexID & 1) << 2);
    const float y        = -1.0 + float((gl_VertexID & 2) << 1);
    gl_Position          = vec4(x, y, 0, 1);
    out_Cube_UV          = mat3(invView) * (invProj * gl_Position).xyz;
}
