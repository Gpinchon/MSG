R""(
uniform sampler2D   in_Texture_Color;

in vec2             frag_UV;

void HZB()
{
    float Depth0 = textureOffset(in_Texture_Color, frag_UV, ivec2(0, 0), 0).r;
    float Depth1 = textureOffset(in_Texture_Color, frag_UV, ivec2(1, 0), 0).r;
    float Depth2 = textureOffset(in_Texture_Color, frag_UV, ivec2(1, 1), 0).r;
    float Depth3 = textureOffset(in_Texture_Color, frag_UV, ivec2(0, 1), 0).r;
    gl_FragDepth = max(max(Depth0, Depth1), max(Depth2, Depth3));
}

)""