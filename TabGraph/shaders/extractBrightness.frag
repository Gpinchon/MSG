R""(
layout(location = 0) out vec4	out_Buffer0;

uniform sampler2D	in_Buffer0;

in vec2	frag_UV;

#define Luminance(linearColor) dot(linearColor, vec3(0.299, 0.587, 0.114))

void ExtractBrightness() {
	vec4 colorSample = texture(in_Buffer0, frag_UV);
	out_Buffer0 = /*max(colorSample - 1, 0) + */colorSample * Luminance(colorSample.rgb);
}
)""