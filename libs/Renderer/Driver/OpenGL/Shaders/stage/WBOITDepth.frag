//////////////////////////////////////// INCLUDES
#include <BRDFInputs.glsl>
#include <Bindings.glsl>
#include <Camera.glsl>
#include <FrameInfo.glsl>
#include <Functions.glsl>
#include <MaterialInputs.glsl>
//////////////////////////////////////// INCLUDES

//////////////////////////////////////// STAGE INPUTS
layout(location = 0) in vec3 in_WorldPosition;
layout(location = 1) in vec3 in_WorldNormal;
layout(location = 2) in vec3 in_WorldTangent;
layout(location = 3) in vec3 in_WorldBitangent;
layout(location = 4) in vec2 in_TexCoord[ATTRIB_TEXCOORD_COUNT];
layout(location = 4 + ATTRIB_TEXCOORD_COUNT) in vec3 in_Color;
layout(location = 4 + ATTRIB_TEXCOORD_COUNT + 1) noperspective in vec3 in_NDCPosition;
layout(location = 4 + ATTRIB_TEXCOORD_COUNT + 2) in vec4 in_Position;
layout(location = 4 + ATTRIB_TEXCOORD_COUNT + 3) in vec4 in_Position_Previous;
//////////////////////////////////////// STAGE INPUTS

//////////////////////////////////////// STAGE OUTPUTS
// none, only depth
//////////////////////////////////////// STAGE OUTPUTS

//////////////////////////////////////// UNIFORMS
layout(binding = UBO_FRAME_INFO) uniform FrameInfoBlock
{
    FrameInfo u_FrameInfo;
};
layout(binding = UBO_CAMERA) uniform CameraBlock
{
    Camera u_Camera;
    Camera u_Camera_Previous;
};
//////////////////////////////////////// UNIFORMS

void main()
{
    const vec4 textureSamplesMaterials[] = SampleTexturesMaterial(in_TexCoord);
    const BRDF brdf                      = GetBRDF(textureSamplesMaterials, in_Color);
    if (brdf.transparency <= 0.003)
        discard;
    gl_FragDepth = in_NDCPosition.z * 0.5f + 0.5f;
}