#include <Lights.glsl>
#include <Transform.glsl>

out gl_PerVertex
{
    vec4 gl_Position;
};
layout(location = 0) out vec3 out_WorldPosition;
layout(location = 1) out vec3 out_WorldNormal;
layout(location = 10) noperspective out vec3 out_NDCPosition;

layout(binding = 0) uniform CameraBlock
{
    // TransformUBO transform;
    mat4 projection;
    mat4 view;
}
u_Camera;
layout(binding = 1) uniform TransformBlock
{
    Transform u_Transform;
};

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec4 in_Tangent;
layout(location = 3) in vec2 in_TexCoord_0;
layout(location = 4) in vec2 in_TexCoord_1;
layout(location = 5) in vec2 in_TexCoord_2;
layout(location = 6) in vec2 in_TexCoord_3;
layout(location = 7) in vec3 in_Color;
layout(location = 8) in uvec4 in_Joints;
layout(location = 9) in vec4 in_Weights;

void main()
{
    mat4x4 MVP        = u_Camera.projection * u_Camera.view;
    vec4 worldPos     = u_Transform.modelMatrix * vec4(in_Position, 1);
    out_WorldNormal   = (u_Transform.normalMatrix * vec4(in_Normal, 0)).xyz;
    out_WorldPosition = worldPos.xyz;
    gl_Position       = MVP * worldPos;
    out_NDCPosition   = gl_Position.xyz / gl_Position.w;
}
