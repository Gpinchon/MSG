#include <Exposure.glsl>
#include <ToneMapping.glsl>

/** @ref https://bruop.github.io/exposure/ */

layout(binding = 0) uniform sampler2D sampler_Input;

layout(binding = 0) uniform Parameters
{
    AutoExposureSettings u_Parameters;
};

layout(location = 0) in invariant vec2 in_UV;

layout(location = 0) out float out_Luminance;

void main()
{
    out_Luminance = log(0.5);
    vec4 hdrColor = texture(sampler_Input, in_UV);
    if (hdrColor.a > 0.03) {
        float lum     = Luminance(hdrColor.rgb) + EPSILON;
        out_Luminance = clamp(log(lum),
            u_Parameters.minLogLum,
            u_Parameters.maxLogLum);
    }
}
