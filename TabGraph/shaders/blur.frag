R""(
uniform sampler2D	in_Texture_Color;
uniform vec2		in_Direction;

in vec2				frag_UV;

out vec4			out_Color;

void Blur()
{
	out_Color = vec4(0);
	vec2 offset1 = in_Direction / 1024.f;
	vec2 offset2 = (in_Direction * 2.f) / 1024.f;
	out_Color += texture2D(in_Texture_Color, frag_UV) * 0.250301f;
	out_Color += texture2D(in_Texture_Color, frag_UV + offset1) * 0.221461f;
	out_Color += texture2D(in_Texture_Color, frag_UV - offset1) * 0.221461f;
	out_Color += texture2D(in_Texture_Color, frag_UV + offset2) * 0.153388f;
	out_Color += texture2D(in_Texture_Color, frag_UV - offset2) * 0.153388f;
}

)""