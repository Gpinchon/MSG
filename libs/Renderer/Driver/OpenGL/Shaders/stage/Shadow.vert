#include <Bindings.glsl>
#include <Camera.glsl>
#include <Lights.glsl>
#include <MeshSkin.glsl>
#include <Transform.glsl>

layout(binding = UBO_CAMERA) uniform CameraBlock
{
    Camera u_Camera;
};
layout(binding = UBO_TRANSFORM) uniform TransformBlock
{
    Transform u_Transform;
};

layout(std430, binding = SSBO_MESH_SKIN) readonly buffer MeshSkinBlock
{
    mat4x4 ssbo_MeshSkinjoints[];
};

layout(location = ATTRIB_POSITION) in vec3 in_Position;
layout(location = ATTRIB_TEXCOORD) in vec2 in_TexCoord[ATTRIB_TEXCOORD_COUNT];
layout(location = ATTRIB_JOINTS) in vec4 in_Joints;
layout(location = ATTRIB_WEIGHTS) in vec4 in_Weights;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) out vec3 out_WorldPosition;
layout(location = 4) out vec2 out_TexCoord[ATTRIB_TEXCOORD_COUNT];

void main()
{
    mat4x4 modelMatrix;
    if (ssbo_MeshSkinjoints.length() > 0) {
        mat4x4 skinMatrix = in_Weights[0] * ssbo_MeshSkinjoints[int(in_Joints[0])]
            + in_Weights[1] * ssbo_MeshSkinjoints[int(in_Joints[1])]
            + in_Weights[2] * ssbo_MeshSkinjoints[int(in_Joints[2])]
            + in_Weights[3] * ssbo_MeshSkinjoints[int(in_Joints[3])];
        modelMatrix  = u_Transform.modelMatrix * skinMatrix;
    } else {
        modelMatrix  = u_Transform.modelMatrix;
    }

    mat4x4 VP     = u_Camera.projection * u_Camera.view;
    vec4 worldPos = modelMatrix * vec4(in_Position, 1);

    gl_Position       = VP * worldPos;
    out_WorldPosition = worldPos.xyz;
    for (uint i = 0; i < in_TexCoord.length(); ++i) {
        out_TexCoord[i] = in_TexCoord[i];
    }
}
