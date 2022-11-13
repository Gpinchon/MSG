R""(
layout(location = 0) out vec4	out_ColorBuffer0;
layout(location = 1) out vec4	out_ColorBuffer1;
layout(location = 2) out vec4	out_ColorBuffer2;
layout(location = 3) out vec4	out_ColorBuffer3;
layout(location = 4) out vec4	out_ColorBuffer4;
layout(location = 5) out vec4	out_ColorBuffer5;
layout(location = 6) out vec4	out_ColorBuffer6;

uniform sampler2D	in_ColorBuffer0;
uniform sampler2D	in_ColorBuffer1;
uniform sampler2D	in_ColorBuffer2;
uniform sampler2D	in_ColorBuffer3;
uniform sampler2D	in_ColorBuffer4;
uniform sampler2D	in_ColorBuffer5;
uniform sampler2D	in_ColorBuffer6;
uniform sampler2D	in_DepthBuffer;

uniform bool		in_UseColorBuffer0 = false;
uniform bool		in_UseColorBuffer1 = false;
uniform bool		in_UseColorBuffer2 = false;
uniform bool		in_UseColorBuffer3 = false;
uniform bool		in_UseColorBuffer4 = false;
uniform bool		in_UseColorBuffer5 = false;
uniform bool		in_UseColorBuffer6 = false;
uniform bool		in_UseDepthBuffer = false;

in vec2	frag_UV;

void PassThrough()
{
	out_ColorBuffer0 = texture(in_ColorBuffer0, frag_UV);
	out_ColorBuffer1 = texture(in_ColorBuffer1, frag_UV);
	out_ColorBuffer2 = texture(in_ColorBuffer2, frag_UV);
	out_ColorBuffer3 = texture(in_ColorBuffer3, frag_UV);
	out_ColorBuffer4 = texture(in_ColorBuffer4, frag_UV);
	out_ColorBuffer5 = texture(in_ColorBuffer5, frag_UV);
	out_ColorBuffer6 = texture(in_ColorBuffer6, frag_UV);
	gl_FragDepth = texture(in_DepthBuffer, frag_UV).r;
	if (!in_UseColorBuffer0) out_ColorBuffer0 = vec4(0);
	if (!in_UseColorBuffer1) out_ColorBuffer1 = vec4(0);
	if (!in_UseColorBuffer2) out_ColorBuffer2 = vec4(0);
	if (!in_UseColorBuffer3) out_ColorBuffer3 = vec4(0);
	if (!in_UseColorBuffer4) out_ColorBuffer4 = vec4(0);
	if (!in_UseColorBuffer5) out_ColorBuffer5 = vec4(0);
	if (!in_UseColorBuffer6) out_ColorBuffer6 = vec4(0);
	if (!in_UseDepthBuffer) gl_FragDepth = 0;
}

)""