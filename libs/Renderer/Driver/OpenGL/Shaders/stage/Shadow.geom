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

bool TriangleIntersectsBox(IN(vec3) a_Triangle[3], IN(vec3) a_BoxCenter, IN(vec3) a_BoxExtents)
{
    vec3 bMin = a_BoxCenter - a_BoxExtents;
    vec3 bMax = a_BoxCenter + a_BoxExtents;
    vec3 tMin = a_Triangle[0];
    vec3 tMax = a_Triangle[0];
    for (uint vI = 1; vI < a_Triangle.length(); vI++) {
        tMin = min(tMin, a_Triangle[vI]);
        tMax = max(tMax, a_Triangle[vI]);
    }
    return tMin.x <= bMax.x
        && tMax.x >= bMin.x
        && tMin.y <= bMax.y
        && tMax.y >= bMin.y
        && tMin.z <= bMax.z
        && tMax.z >= bMin.z;
}

void main()
{
    bool emittedVertices = false;
    float minDepth       = 10000;
    float maxDepth       = 0;
    for (int viewportI = 0; viewportI < ssbo_ShadowCaster.viewportCount; viewportI++) {
        Camera viewport = ssbo_ShadowViewports[viewportI];
        gl_Layer        = viewportI;
        vec4 triangle[3];
        vec3 triangleNDC[3];
        for (int vertexI = 0; vertexI < gs_in.length(); vertexI++) {
            vec4 viewPos         = viewport.view * gs_in[vertexI].worldPosition;
            triangle[vertexI]    = viewport.projection * viewPos;
            triangleNDC[vertexI] = triangle[vertexI].xyz / triangle[vertexI].w;
        }
        if (!TriangleIntersectsBox(triangleNDC, vec3(0, 0, 0), vec3(1, 1, 1)))
            continue;
        emittedVertices = true;
        for (int vertexI = 0; vertexI < gs_in.length(); vertexI++) {
            gl_Position = triangle[vertexI];
            for (uint tc = 0; tc < gs_in[vertexI].texCoord.length(); tc++)
                out_TexCoord[tc] = gs_in[vertexI].texCoord[tc];
#if SHADOW_CUBE
            out_Depth = distance(viewport.position, gs_in[vertexI].worldPosition.xyz);
            out_Depth = normalizeValue(out_Depth, viewport.zNear, viewport.zFar);
            minDepth  = min(minDepth, out_Depth);
            maxDepth  = max(maxDepth, out_Depth);
#else
            out_Depth = triangleNDC[vertexI].z * 0.5 + 0.5;
            if (out_Depth >= 0 && all(greaterThanEqual(triangleNDC[vertexI].xy, vec2(-1))) && all(lessThanEqual(triangleNDC[vertexI].xy, vec2(1)))) {
                minDepth = min(minDepth, out_Depth);
                maxDepth = max(maxDepth, out_Depth);
            }
#endif
            out_Depth = normalizeValue(out_Depth, ssbo_MinDepth_Prev, ssbo_MaxDepth_Prev);
            out_Depth += ssbo_ShadowCaster.bias;
            out_DepthRange = abs(viewport.zFar - viewport.zNear);
            EmitVertex();
        }
        EndPrimitive();
    }
    if (emittedVertices) {
        atomicMin(ssbo_MinDepth, floatBitsToUint(minDepth));
        atomicMax(ssbo_MaxDepth, floatBitsToUint(maxDepth));
    }
}