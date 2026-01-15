#include <Bindings.glsl>
#include <Camera.glsl>
#include <Lights.glsl>

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

layout(std430, binding = SSBO_SHADOW_DEPTH_RANGE) restrict buffer DepthRangeSSBO
{
    float ssbo_MinDepth;
    float ssbo_MaxDepth;
    uint ssbo_DepthRange_Padding[2];
};

layout(std430, binding = SSBO_SHADOW_CASTERS) readonly buffer ShadowCasterSSBO
{
    ShadowCaster ssbo_ShadowCaster;
};

layout(std430, binding = SSBO_SHADOW_VIEWPORTS) readonly buffer ViewportsSSBO
{
    Camera ssbo_ShadowViewports[];
};

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
layout(location = 0) out float out_DepthRange;
layout(location = 1) out float out_UnclampedDepth;
layout(location = 2) out vec2 out_TexCoord[ATTRIB_TEXCOORD_COUNT];

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
        for (int vertexI = 0; vertexI < gs_in.length(); vertexI++) {
#if SHADOW_CUBE
            float depth = distance(viewport.position, gs_in[vertexI].worldPosition.xyz);
            depth       = normalizeValue(depth, viewport.zNear, viewport.zFar);
#else
            float depth = triangleNDC[vertexI].z * 0.5 + 0.5;
#endif
            out_UnclampedDepth = depth;
            depth              = normalizeValue(depth, ssbo_MinDepth, ssbo_MaxDepth);
            // depth += ssbo_ShadowCaster.bias;
            // write outputs
            gl_Position.xy = triangle[vertexI].xy;
            gl_Position.z  = (depth * 2 - 1) * triangle[vertexI].w;
            gl_Position.w  = triangle[vertexI].w;
            out_DepthRange = abs(viewport.zFar - viewport.zNear);
            for (uint tc = 0; tc < gs_in[vertexI].texCoord.length(); tc++)
                out_TexCoord[tc] = gs_in[vertexI].texCoord[tc];
            EmitVertex();
        }
        EndPrimitive();
    }
}