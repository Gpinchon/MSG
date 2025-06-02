#include <Bindings.glsl>
#include <Camera.glsl>
#include <Lights.glsl>
#include <MeshSkin.glsl>
#include <ShadowData.glsl>
#include <Transform.glsl>

layout(binding = UBO_TRANSFORM) uniform TransformBlock
{
    Transform u_Transform;
};

layout(std430, binding = SSBO_SHADOW_DATA) readonly buffer DataBlock
{
    ShadowBase ssbo_ShadowData;
};

layout(std430, binding = SSBO_SHADOW_VIEWPORTS) readonly buffer ViewportBlock
{
    Camera ssbo_ShadowViewport;
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

layout(location = 0) out float out_Depth;
layout(location = 4) out vec2 out_TexCoord[ATTRIB_TEXCOORD_COUNT];

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
    vec4 worldPos   = modelMatrix * vec4(in_Position, 1);
    vec4 viewPos    = ssbo_ShadowViewport.view * worldPos;
    vec4 NDCPosProj = ssbo_ShadowViewport.projection * viewPos;
    gl_Position     = NDCPosProj;
    for (uint i = 0; i < in_TexCoord.length(); ++i) {
        out_TexCoord[i] = in_TexCoord[i];
    }
#if SHADOW_CUBE
    out_Depth = distance(ssbo_ShadowViewport.position, worldPos.xyz);
    out_Depth = normalizeValue(out_Depth, ssbo_ShadowViewport.zNear, ssbo_ShadowViewport.zFar);
#else
    out_Depth = NDCPosProj.z / NDCPosProj.w * 0.5 + 0.5;
#endif
    out_Depth += ssbo_ShadowData.bias;
}
