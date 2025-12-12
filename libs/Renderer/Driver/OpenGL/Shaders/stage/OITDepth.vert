#include <Bindings.glsl>
#include <Camera.glsl>
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

layout(std430, binding = SSBO_MESH_SKIN_PREV) readonly buffer MeshSkinBlockPrevious
{
    mat4x4 ssbo_MeshSkinjoints_Previous[];
};

layout(location = ATTRIB_POSITION) in vec3 in_Position;
layout(location = ATTRIB_TEXCOORD) in vec2 in_TexCoord[ATTRIB_TEXCOORD_COUNT];
layout(location = ATTRIB_JOINTS) in vec4 in_Joints;
layout(location = ATTRIB_WEIGHTS) in vec4 in_Weights;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 4) out vec2 out_TexCoord[ATTRIB_TEXCOORD_COUNT];
layout(location = 4 + ATTRIB_TEXCOORD_COUNT + 1) noperspective out vec3 out_NDCPosition;

void main()
{
    mat4x4 modelMatrix;
    mat4x4 normalMatrix;
#if SKINNED
    mat4x4 skinMatrix = in_Weights[0] * ssbo_MeshSkinjoints[int(in_Joints[0])]
        + in_Weights[1] * ssbo_MeshSkinjoints[int(in_Joints[1])]
        + in_Weights[2] * ssbo_MeshSkinjoints[int(in_Joints[2])]
        + in_Weights[3] * ssbo_MeshSkinjoints[int(in_Joints[3])];
    modelMatrix  = u_Transform.modelMatrix * skinMatrix;
    normalMatrix = inverse(transpose(modelMatrix));
#else
    modelMatrix  = u_Transform.modelMatrix;
    normalMatrix = u_Transform.normalMatrix;
#endif
    mat4x4 VP       = u_Camera.projection * u_Camera.view;
    vec4 worldPos   = modelMatrix * vec4(in_Position, 1);
    gl_Position     = VP * worldPos;
    out_NDCPosition = gl_Position.xyz / gl_Position.w;
    for (uint i = 0; i < in_TexCoord.length(); ++i) {
        out_TexCoord[i] = in_TexCoord[i];
    }
}
