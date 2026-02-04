#include <Bindings.glsl>
#include <Camera.glsl>

layout(triangles) in;
layout(triangle_strip, max_vertices = 21) out;

layout(binding = UBO_CAMERA) uniform CameraBlock
{
    Camera u_Camera;
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
layout(location = 0) out vec2 out_TexCoord[ATTRIB_TEXCOORD_COUNT];

void main()
{
    for (int samplerI = 0; samplerI < SAMPLERS_MATERIAL_COUNT; samplerI++) {
        gl_Layer = samplerI;
        for (int vertexI = 0; vertexI < gs_in.length(); vertexI++) {
            vec4 viewPos = u_Camera.view * gs_in[vertexI].worldPosition;
            gl_Position  = u_Camera.projection * viewPos;
            out_TexCoord = gs_in[vertexI].texCoord;
            EmitVertex();
        }
        EndPrimitive();
    }
}