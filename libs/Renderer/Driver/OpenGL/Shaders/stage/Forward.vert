#include <Bindings.glsl>
#include <Camera.glsl>
#include <Lights.glsl>
#include <MeshSkin.glsl>
#include <Transform.glsl>

layout(binding = UBO_CAMERA) uniform CameraBlock
{
    Camera u_Camera;
    Camera u_Camera_Previous;
};
layout(binding = UBO_TRANSFORM) uniform TransformBlock
{
    Transform u_Transform;
    Transform u_Transform_Previous;
};

layout(std430, binding = SSBO_MESH_SKIN) readonly buffer MeshSkinBlock
{
    mat4x4 ssbo_MeshSkinjoints[];
};

layout(std430, binding = SSBO_MESH_SKIN_PREV) readonly buffer MeshSkinBlockPrevious
{
    mat4x4 ssbo_MeshSkinjoints_Previous[];
};

layout(location = ATTRIB_POSITION) in vec3 in_Position;
layout(location = ATTRIB_NORMAL) in vec3 in_Normal;
layout(location = ATTRIB_TANGENT) in vec4 in_Tangent;
layout(location = ATTRIB_TEXCOORD) in vec2 in_TexCoord[ATTRIB_TEXCOORD_COUNT];
layout(location = ATTRIB_COLOR) in vec3 in_Color;
layout(location = ATTRIB_JOINTS) in vec4 in_Joints;
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
layout(location = 4 + ATTRIB_TEXCOORD_COUNT + 2) out vec4 out_Position;
layout(location = 4 + ATTRIB_TEXCOORD_COUNT + 3) out vec4 out_Position_Previous;

void main()
{
    mat4x4 modelMatrix;
    mat4x4 normalMatrix;
    if (ssbo_MeshSkinjoints.length() > 0) {
        mat4x4 skinMatrix = in_Weights[0] * ssbo_MeshSkinjoints[int(in_Joints[0])]
            + in_Weights[1] * ssbo_MeshSkinjoints[int(in_Joints[1])]
            + in_Weights[2] * ssbo_MeshSkinjoints[int(in_Joints[2])]
            + in_Weights[3] * ssbo_MeshSkinjoints[int(in_Joints[3])];
        modelMatrix  = u_Transform.modelMatrix * skinMatrix;
        normalMatrix = inverse(transpose(modelMatrix));
    } else {
        modelMatrix  = u_Transform.modelMatrix;
        normalMatrix = u_Transform.normalMatrix;
    }

    mat4x4 modelMatrix_Previous;
    if (ssbo_MeshSkinjoints_Previous.length() > 0) {
        mat4x4 skinMatrix = in_Weights[0] * ssbo_MeshSkinjoints_Previous[int(in_Joints[0])]
            + in_Weights[1] * ssbo_MeshSkinjoints_Previous[int(in_Joints[1])]
            + in_Weights[2] * ssbo_MeshSkinjoints_Previous[int(in_Joints[2])]
            + in_Weights[3] * ssbo_MeshSkinjoints_Previous[int(in_Joints[3])];
        modelMatrix_Previous = u_Transform_Previous.modelMatrix * skinMatrix;
    } else {
        modelMatrix_Previous = u_Transform_Previous.modelMatrix;
    }
    mat4x4 VP_Previous     = u_Camera_Previous.projection * u_Camera_Previous.view;
    vec4 worldPos_Previous = modelMatrix_Previous * vec4(in_Position, 1);
    out_Position_Previous  = VP_Previous * worldPos_Previous;

    mat4x4 VP     = u_Camera.projection * u_Camera.view;
    vec4 worldPos = modelMatrix * vec4(in_Position, 1);

    out_Position       = VP * worldPos;
    out_WorldPosition  = worldPos.xyz;
    out_WorldNormal    = (normalMatrix * vec4(in_Normal, 0)).xyz;
    out_WorldTangent   = (normalMatrix * vec4(in_Tangent.xyz * in_Tangent.w, 0)).xyz;
    out_WorldBitangent = cross(out_WorldNormal, out_WorldTangent);
    out_Color          = in_Color;
    out_NDCPosition    = out_Position.xyz / out_Position.w;
    gl_Position        = out_Position;
    for (uint i = 0; i < in_TexCoord.length(); ++i) {
        out_TexCoord[i] = in_TexCoord[i];
    }
}
