#include <Bindings.glsl>
#include <MeshSkin.glsl>
#include <Transform.glsl>

layout(binding = UBO_TRANSFORM) uniform TransformBlock
{
    Transform u_Transform;
};

layout(std430, binding = SSBO_MESH_SKIN) readonly buffer MeshSkinSSBGO
{
    mat4x4 ssbo_MeshSkinjoints[];
};

layout(location = ATTRIB_POSITION) in vec3 in_Position;
layout(location = ATTRIB_TEXCOORD) in vec2 in_TexCoord[ATTRIB_TEXCOORD_COUNT];
layout(location = ATTRIB_JOINTS) in vec4 in_Joints;
layout(location = ATTRIB_WEIGHTS) in vec4 in_Weights;

out VS_OUT
{
    vec4 worldPosition;
    vec2 texCoord[ATTRIB_TEXCOORD_COUNT];
}
vs_out;

void main()
{
    mat4x4 modelMatrix;
    if (ssbo_MeshSkinjoints.length() > 0) {
        mat4x4 skinMatrix = in_Weights[0] * ssbo_MeshSkinjoints[int(in_Joints[0])]
            + in_Weights[1] * ssbo_MeshSkinjoints[int(in_Joints[1])]
            + in_Weights[2] * ssbo_MeshSkinjoints[int(in_Joints[2])]
            + in_Weights[3] * ssbo_MeshSkinjoints[int(in_Joints[3])];
        modelMatrix = u_Transform.modelMatrix * skinMatrix;
    } else {
        modelMatrix = u_Transform.modelMatrix;
    }
    vs_out.worldPosition = modelMatrix * vec4(in_Position, 1);
    for (uint i = 0; i < in_TexCoord.length(); ++i) {
        vs_out.texCoord[i] = in_TexCoord[i];
    }
}
