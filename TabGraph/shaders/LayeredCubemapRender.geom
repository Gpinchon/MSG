R""(
layout(triangles, invocations = 6) in;
layout(triangle_strip, max_vertices = 18) out;

void LayeredCubemapRender(void) {
    for (int i = 0; i < gl_in.length(); i++) {
        gl_Layer = gl_InvocationID;
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}
)""