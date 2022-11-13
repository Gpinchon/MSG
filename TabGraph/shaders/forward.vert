R""(
//Pass
#define DeferredGeometry		0
#define DeferredLighting		1
#define DeferredMaterial		2
#define ForwardTransparent		3
#define ForwardOpaque			4
#define ShadowDepth				5
#define GeometryPostTreatment	6

struct t_Matrix {
	mat4	Model;
	mat3	Normal;
};

struct t_Joint {
	mat4	Matrix;
};

struct t_CameraMatrix {
	mat4	View;
	mat4	Projection;
};

struct t_Camera {
	vec3			Position;
	t_CameraMatrix	Matrix;
	t_CameraMatrix	InvMatrix;
};

/*struct	t_Vert {
	vec3	Position;
	vec3	Normal;
	vec2	UV;
};*/

layout(location = 0) in vec3	in_Position;
layout(location = 1) in vec3	in_Normal;
layout(location = 2) in vec3	in_Tangent;
layout(location = 3) in vec2	in_TexCoord_0;
layout(location = 4) in vec2	in_TexCoord_1;
layout(location = 5) in vec4	in_Color_0;
layout(location = 6) in vec4	in_Joints_0;
layout(location = 7) in vec4	in_Weight_0;

uniform t_Camera				PrevCamera;
uniform t_Camera				Camera;
uniform t_Matrix				PrevMatrix;
uniform t_Matrix				Matrix;
uniform mat4					SkinTransform;
uniform mat4					SkinPrevTransform;
uniform samplerBuffer			Joints;
uniform samplerBuffer			PrevJoints;
uniform bool					Skinned = false;
uniform mat3					UVTransform = mat3(1);

out VertexOutput {
	vec3	WorldPosition;
	vec3	WorldNormal;
	vec2	TexCoord;
} Output;

out vec3 ModelPosition;
out vec4 Position;
out vec4 PreviousPosition;
out float CameraSpaceDepth;
#if Pass == ForwardTransparent || Pass == ForwardOpaque
out vec3 VertexDiffuse;
#if OpacityMode == Blend
out vec3 BackVertexDiffuse;
#endif //OpacityMode == Blend
#endif

bool _WorldPositionSet = false;

void SetWorldPosition(in vec3 worldPosition)
{
	Output.WorldPosition = worldPosition;
	_WorldPositionSet = true;
}

vec3 WorldPosition()
{
	if (!_WorldPositionSet)
		SetWorldPosition(vec3(Matrix.Model * vec4(in_Position, 1.0)));
	return Output.WorldPosition;
}

bool _WorldNormalSet = false;

void SetWorldNormal(in vec3 worldNormal)
{
	Output.WorldNormal = worldNormal;
	_WorldNormalSet = true;
}

vec3 WorldNormal()
{
	if (!_WorldNormalSet)
		SetWorldNormal(mat3(Matrix.Normal) * in_Normal);
	return Output.WorldNormal;
}

bool _TexCoordSet = false;

void SetTexCoord(in vec2 texCoord)
{
	Output.TexCoord = texCoord;
	_TexCoordSet = true;
}

vec2 TexCoord()
{
	if (!_TexCoordSet)
		SetTexCoord(in_TexCoord_0);
	return Output.TexCoord;
}

bool _ClipSpacePositionSet = false;

void SetClipSpacePosition(in vec4 clipSpacePosition)
{
	gl_Position = clipSpacePosition;
	_ClipSpacePositionSet = true;
}

vec4 ClipSpacePosition()
{
	if (!_ClipSpacePositionSet)
		SetClipSpacePosition(Camera.Matrix.Projection * Camera.Matrix.View * vec4(WorldPosition(), 1));
	return gl_Position;
}

mat4	GetJointMatrix(const in int index)
{
	return mat4(
		texelFetch(Joints, index * 4 + 0),
		texelFetch(Joints, index * 4 + 1),
		texelFetch(Joints, index * 4 + 2),
		texelFetch(Joints, index * 4 + 3)
	);
}

mat4	GetPrevJointMatrix(const in int index)
{
	return mat4(
		texelFetch(PrevJoints, index * 4 + 0),
		texelFetch(PrevJoints, index * 4 + 1),
		texelFetch(PrevJoints, index * 4 + 2),
		texelFetch(PrevJoints, index * 4 + 3)
	);
}

void	FillVertexData()
{
	mat4 Joint[4];
	Joint[0] = GetJointMatrix(int(in_Joints_0.x));
	Joint[1] = GetJointMatrix(int(in_Joints_0.y));
	Joint[2] = GetJointMatrix(int(in_Joints_0.z));
	Joint[3] = GetJointMatrix(int(in_Joints_0.w));
	mat4 PrevJoint[4];
	PrevJoint[0] = GetPrevJointMatrix(int(in_Joints_0.x));
	PrevJoint[1] = GetPrevJointMatrix(int(in_Joints_0.y));
	PrevJoint[2] = GetPrevJointMatrix(int(in_Joints_0.z));
	PrevJoint[3] = GetPrevJointMatrix(int(in_Joints_0.w));
	if (Skinned) {
		mat4 SkinMatrix =
        in_Weight_0.x * Joint[0] +
        in_Weight_0.y * Joint[1] +
        in_Weight_0.z * Joint[2] +
        in_Weight_0.w * Joint[3];
		mat4 PrevSkinMatrix =
        in_Weight_0.x * PrevJoint[0] +
        in_Weight_0.y * PrevJoint[1] +
        in_Weight_0.z * PrevJoint[2] +
        in_Weight_0.w * PrevJoint[3];
        mat4 ModelMatrix = Matrix.Model * SkinTransform * SkinMatrix;
		SetWorldPosition(vec3(ModelMatrix * vec4(in_Position, 1.0)));
		SetWorldNormal(normalize(mat3(inverse(transpose(ModelMatrix))) * in_Normal));
		mat4 PrevModelMatrix = PrevMatrix.Model * SkinPrevTransform * PrevSkinMatrix;
		PreviousPosition = PrevCamera.Matrix.Projection * PrevCamera.Matrix.View * PrevModelMatrix * vec4(in_Position, 1.0);
	}
	else {
		SetWorldPosition(WorldPosition());
		SetWorldNormal(normalize(WorldNormal()));
		PreviousPosition = PrevCamera.Matrix.Projection * PrevCamera.Matrix.View * PrevMatrix.Model * vec4(in_Position, 1.0);
	}
	ModelPosition = in_Position;
	SetTexCoord((UVTransform * vec3(TexCoord(), 1)).xy);
	vec4 cameraSpacePosition = Camera.Matrix.View * vec4(WorldPosition(), 1);
	CameraSpaceDepth = cameraSpacePosition.z;
	SetClipSpacePosition(Position = (Camera.Matrix.Projection * cameraSpacePosition));
#if Pass == ForwardTransparent || Pass == ForwardOpaque
	VertexDiffuse = SampleSH(WorldNormal());
	#if OpacityMode == Blend
	BackVertexDiffuse = SampleSH(-WorldNormal());
	#endif
#endif
}

)""