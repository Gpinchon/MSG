//////////////////////////////////////// INCLUDES
#include <BRDF.glsl>
#include <Bindings.glsl>
#include <Camera.glsl>
#include <MaterialInputs.glsl>
//////////////////////////////////////// INCLUDES

//////////////////////////////////////// STAGE INPUTS
layout(location = 0) in float in_Depth;
layout(location = 4) in vec2 in_TexCoord[ATTRIB_TEXCOORD_COUNT];
//////////////////////////////////////// STAGE INPUTS

//////////////////////////////////////// STAGE OUTPUTS
layout(location = 0) out vec2 out_Moments;
//////////////////////////////////////// STAGE OUTPUTS

//////////////////////////////////////// UNIFORMS
// None
//////////////////////////////////////// UNIFORMS

//////////////////////////////////////// SSBOS
// None
//////////////////////////////////////// SSBOS

vec2 ComputeMoments(float Depth)
{
    vec2 Moments;
    // First moment is the depth itself.
    Moments.x = Depth;
    // Compute partial derivatives of depth.
    float dx = dFdx(Depth);
    float dy = dFdy(Depth);
    // Compute second moment over the pixel extents.
    Moments.y = Depth * Depth + 0.25 * (dx * dx + dy * dy);
    return Moments;
}

void main()
{
    if (GetBRDF(SampleTexturesMaterial(in_TexCoord), vec3(0)).transparency < 0.9)
        discard;
    out_Moments = ComputeMoments(in_Depth);
}