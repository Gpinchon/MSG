#include <Exposure.glsl>
#include <ToneMapping.glsl>

layout(binding = 0, rgba16f) restrict uniform image2D img_Target;

layout(binding = 0) uniform AutoExposureSettingsBlock
{
    AutoExposureSettings u_AutoExposureSettings;
};
layout(binding = 1) uniform ToneMappingSettingsBlock
{
    ToneMappingSettings u_ToneMappingSettings;
};
layout(binding = 2) uniform LuminanceBlock
{
    float u_Luminance;
    uint _padding[3];
};

void main()
{
    ivec2 coord = ivec2(gl_FragCoord.xy);
    vec4 color  = imageLoad(img_Target, coord);

    /** @ref Advanced Graphics Programming Using OpenGL, Tom McReynolds & David Blythe */
    ToneMappingSettings toneMappingSettings = u_ToneMappingSettings;
#if AUTO_EXPOSURE == 1
    toneMappingSettings.exposure = toneMappingSettings.exposure + (u_AutoExposureSettings.key / u_Luminance * Luminance(color.rgb) - 1.f);
#endif
    color.rgb = ApplyToneMapping(color.rgb, toneMappingSettings);
    imageStore(img_Target, coord, saturate(color));
}