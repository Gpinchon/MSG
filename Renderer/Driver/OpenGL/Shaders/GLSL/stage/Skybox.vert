#include <Bindings.glsl>
#include <Camera.glsl>
#include <Functions.glsl>

layout(binding = UBO_CAMERA) uniform CameraBlock
{
    Camera u_Camera;
    Camera u_Camera_Previous;
};

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) out vec3 out_Cube_UV;
layout(location = 1) out vec4 out_Position;
layout(location = 2) out vec4 out_Position_Previous;

void ComputePosition(IN(Camera) a_Camera, OUT(vec4) a_Position) {
    const mat4x4 invProj = inverse(a_Camera.projection);
    const mat4x4 invView = inverse(a_Camera.view);
    const float x        = -1.0 + float((gl_VertexID & 1) << 2);
    const float y        = -1.0 + float((gl_VertexID & 2) << 1);
    a_Position          = vec4(x, y, 0, 1);
}

void ComputeCubeUV(IN(Camera) a_Camera, OUT(vec4) a_Position, OUT(vec3) a_Cube_UV) {
    const mat4x4 invProj = inverse(a_Camera.projection);
    const mat4x4 invView = inverse(a_Camera.view);
    const float x        = -1.0 + float((gl_VertexID & 1) << 2);
    const float y        = -1.0 + float((gl_VertexID & 2) << 1);
    a_Position          = vec4(x, y, 0, 1);
    a_Cube_UV = mat3(invView) * (invProj * a_Position).xyz;
}

void main()
{
    ComputeCubeUV(u_Camera, out_Position,  out_Cube_UV);
    ComputePosition(u_Camera_Previous, out_Position_Previous);
    gl_Position = out_Position;
}
