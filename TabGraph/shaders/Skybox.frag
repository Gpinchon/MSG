R""(
layout(location = 4) out vec4	out_Color;

in VertexOutput {
	vec2	TexCoord;
	vec3	CubeTexCoord;
} Input;

in vec2	frag_UV;

uniform samplerCube Skybox;

void OutputEnv() {
	out_Color = texture(Skybox, Input.CubeTexCoord, 0);
}

)""