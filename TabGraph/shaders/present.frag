R""(
out vec4			out_Color;

uniform sampler2D	in_Texture_Color;
uniform sampler2D	in_Texture_Emissive;
uniform sampler2D	in_Texture_Depth;

in vec2	frag_UV;

vec3 gammaCorrection = vec3(1);

void Present()
{
	out_Color = pow(texture(in_Texture_Color, frag_UV), vec4(gammaCorrection, 1));
	out_Color.rgb += texture(in_Texture_Emissive, frag_UV).rgb;
	gl_FragDepth = texture(in_Texture_Depth, frag_UV).r;
}

)""