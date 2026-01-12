#include <Bindings.glsl>
#include <Camera.glsl>
#include <Lights.glsl>

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

layout(std430, binding = SSBO_SHADOW_DEPTH_RANGE) restrict coherent buffer DepthRangeSSBO
{
    uint ssbo_MinDepth;
    uint ssbo_MaxDepth;
    uint ssbo_DepthRange_Padding[2];
};

layout(std430, binding = SSBO_SHADOW_DEPTH_RANGE + 1) restrict readonly buffer DepthRangeSSBO_Prev
{
    float ssbo_MinDepth_Prev;
    float ssbo_MaxDepth_Prev;
    uint ssbo_DepthRange_Prev_Padding[2];
};

layout(std430, binding = SSBO_SHADOW_CASTERS) readonly buffer ShadowCasterSSBO
{
    ShadowCaster ssbo_ShadowCaster;
};

layout(std430, binding = SSBO_SHADOW_VIEWPORTS) readonly buffer ViewportsSSBO
{
    Camera ssbo_ShadowViewports[];
};

layout(location = 0) out float out_Depth;
layout(location = 1) out float out_DepthRange;
layout(location = 4) out vec2 out_TexCoord[ATTRIB_TEXCOORD_COUNT];

in VS_OUT
{
    vec4 worldPosition;
    vec2 texCoord[ATTRIB_TEXCOORD_COUNT];
}
gs_in[];

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    float prevMinDepth = ssbo_MinDepth_Prev;
    float prevMaxDepth = ssbo_MaxDepth_Prev;
    float minDepth     = 10000;
    float maxDepth     = -10000;
    for (int viewportI = 0; viewportI < ssbo_ShadowCaster.viewportCount; viewportI++) {
        Camera viewport = ssbo_ShadowViewports[viewportI];
        gl_Layer        = viewportI;
        for (int vertexI = 0; vertexI < gs_in.length(); vertexI++) {
            vec4 viewPos = viewport.view * gs_in[vertexI].worldPosition;
            gl_Position  = viewport.projection * viewPos;
            for (uint tc = 0; tc < gs_in[vertexI].texCoord.length(); tc++)
                out_TexCoord[tc] = gs_in[vertexI].texCoord[tc];
#if SHADOW_CUBE
            out_Depth = distance(viewport.position, gs_in[vertexI].worldPosition.xyz);
            out_Depth = normalizeValue(out_Depth, viewport.zNear, viewport.zFar);
#else
            out_Depth = gl_Position.z / gl_Position.w * 0.5 + 0.5;
#endif
            minDepth  = min(minDepth, out_Depth);
            maxDepth  = max(maxDepth, out_Depth);
            out_Depth = normalizeValue(out_Depth, prevMinDepth, prevMaxDepth);
            out_Depth += ssbo_ShadowCaster.bias;
            out_DepthRange = abs(viewport.zFar - viewport.zNear);
            EmitVertex();
        }
        EndPrimitive();
    }
    atomicMin(ssbo_MinDepth, floatBitsToUint(minDepth));
    atomicMax(ssbo_MaxDepth, floatBitsToUint(maxDepth));
}