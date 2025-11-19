#include <Exposure.glsl>
#include <ToneMapping.glsl>

layout(binding = 0, rgba16f) restrict uniform image2D img_Target;

layout(binding = 0) uniform SettingsBlock
{
    AutoExposureSettings u_AutoExposureSettings;
    ColorGradingSettings u_ColorGradingSettings;
    ToneMappingSettings u_ToneMappingSettings;
};
layout(binding = 1) uniform LuminanceBlock
{
    float u_Luminance;
    uint _padding[3];
};

void main()
{
    ivec2 coord = ivec2(gl_FragCoord.xy);
    vec4 color  = imageLoad(img_Target, coord);

    /** @ref Advanced Graphics Programming Using OpenGL, Tom McReynolds & David Blythe */
    ColorGradingSettings colorGradingSettings = u_ColorGradingSettings;
#if AUTO_EXPOSURE == 1
    colorGradingSettings.exposure = colorGradingSettings.exposure + (u_AutoExposureSettings.key / u_Luminance - 1.0f);
#endif
    color.rgb = ApplyColorGrading(color.rgb, colorGradingSettings);
    color.rgb = ApplyToneMapping(color.rgb, u_ToneMappingSettings);
    imageStore(img_Target, coord, saturate(color));
}