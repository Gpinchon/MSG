#include <Bindings.glsl>
#include <Camera.glsl>
#include <Lights.glsl>
#include <Transform.glsl>

layout(binding = UBO_CAMERA) uniform CameraBlock
{
    Camera u_Camera;
};
layout(binding = UBO_TRANSFORM) uniform TransformBlock
{
    Transform u_Transform;
};

layout(location = ATTRIB_POSITION) in vec3 in_Position;
layout(location = ATTRIB_NORMAL) in vec3 in_Normal;
layout(location = ATTRIB_TANGENT) in vec4 in_Tangent;
layout(location = ATTRIB_TEXCOORD) in vec2 in_TexCoord[ATTRIB_TEXCOORD_COUNT];
layout(location = ATTRIB_COLOR) in vec3 in_Color;
layout(location = ATTRIB_JOINTS) in uvec4 in_Joints;
layout(location = ATTRIB_WEIGHTS) in vec4 in_Weights;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) out vec3 out_WorldPosition;
layout(location = 1) out vec3 out_WorldNormal;
layout(location = 2) out vec3 out_WorldTangent;
layout(location = 3) out vec3 out_WorldBitangent;
layout(location = 4) out vec2 out_TexCoord[ATTRIB_TEXCOORD_COUNT];
layout(location = 4 + ATTRIB_TEXCOORD_COUNT) out vec3 out_Color;
layout(location = 4 + ATTRIB_TEXCOORD_COUNT + 1) noperspective out vec3 out_NDCPosition;

void main()
{
    mat4x4 VP          = u_Camera.projection * u_Camera.view;
    vec4 worldPos      = u_Transform.modelMatrix * vec4(in_Position, 1);
    gl_Position        = VP * worldPos;
    out_WorldPosition  = worldPos.xyz;
    out_WorldNormal    = (u_Transform.normalMatrix * vec4(in_Normal, 0)).xyz;
    out_WorldTangent   = (u_Transform.normalMatrix * vec4(in_Tangent.xyz * in_Tangent.w, 0)).xyz;
    out_WorldBitangent = cross(out_WorldNormal, out_WorldTangent);
    out_Color          = in_Color;
    out_NDCPosition    = gl_Position.xyz / gl_Position.w;
    for (uint i = 0; i < in_TexCoord.length(); ++i) {
        out_TexCoord[i] = in_TexCoord[i];
    }
}
