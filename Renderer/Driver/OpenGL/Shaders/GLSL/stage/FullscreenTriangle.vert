out gl_PerVertex
{
    invariant vec4 gl_Position;
};
out invariant vec2 UV;

void main()
{
    float x     = -1.0 + float((gl_VertexID & 1) << 2);
    float y     = -1.0 + float((gl_VertexID & 2) << 1);
    UV.x        = (x + 1.0) * 0.5;
    UV.y        = 1 - (y + 1.0) * 0.5;
    gl_Position = vec4(x, y, 0, 1);
}
