R""(
//layout(location = 0) in vec2	in_Position;

uniform mat4					in_InvProjMatrix;
uniform mat4					in_InvViewMatrix;

out vec2	frag_UV;
out vec3	frag_Cube_UV;

void PassThrough()
{
	//frag_UV = vec2(in_Position.x == -1 ? 0 : 1, in_Position.y == -1 ? 0 : 1);
	//gl_Position = vec4(in_Position, 0, 1);
	//frag_Cube_UV = -mat3(in_InvViewMatrix) * (in_InvProjMatrix * gl_Position).xyz;

	float x = -1.0 + float((gl_VertexID & 1) << 2);
    float y = -1.0 + float((gl_VertexID & 2) << 1);
    frag_UV.x = (x+1.0)*0.5;
    frag_UV.y = (y+1.0)*0.5;
    gl_Position = vec4(x, y, 0, 1);
	frag_Cube_UV = -mat3(in_InvViewMatrix) * (in_InvProjMatrix * gl_Position).xyz;
}

)""