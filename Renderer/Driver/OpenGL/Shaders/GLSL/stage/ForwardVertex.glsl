#include <Lights.glsl>

struct TransformUBO {
    // vec3 position;
    // vec3 scale;
    // vec4 rotation;
    mat4 matrix;
};

out gl_PerVertex
{
    vec4 gl_Position;
};
layout(location = 0) out vec3 out_WorldPosition;
layout(location = 1) noperspective out vec3 out_NDCPosition;

layout(binding = 0) uniform CameraBlock
{
    // TransformUBO transform;
    mat4 projection;
    mat4 view;
}
u_Camera;
layout(binding = 1) uniform TransformBlock
{
    TransformUBO u_ModelTransform;
};

layout(location = 0) in vec3 in_Position;

void main()
{
    mat4x4 MVP        = u_Camera.projection * u_Camera.view;
    vec4 worldPos     = u_ModelTransform.matrix * vec4(in_Position, 1);
    out_WorldPosition = worldPos.xyz;
    gl_Position       = MVP * worldPos;
    out_NDCPosition   = gl_Position.xyz / gl_Position.w;
}
