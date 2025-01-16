out gl_PerVertex
{
    invariant vec4 gl_Position;
};

layout(location = 0) out invariant vec2 out_UV;

void main()
{
    float x     = -1.0 + float((gl_VertexID & 1) << 2);
    float y     = -1.0 + float((gl_VertexID & 2) << 1);
    out_UV.x    = (x + 1.0) * 0.5;
    out_UV.y    = (y + 1.0) * 0.5;
    gl_Position = vec4(x, y, 0, 1);
}
