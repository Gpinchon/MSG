#ifndef TONEMAPPING_GLSL
#define TONEMAPPING_GLSL

#include <Functions.glsl>

#define DEFAULT_GAMMA    2.2f
#define TONEMAP_NONE     -1
#define TONEMAP_ACES     0
#define TONEMAP_REINHARD 1
#define TONEMAP_LOTTES   2
#define TONEMAP_NEUTRAL  3

#ifdef __cplusplus
namespace Msg::Renderer::GLSL {
#endif //__cplusplus
struct LottesSettings {
    float hdrMax;
    float contrast;
    float shoulder;
    float midIn;
    float midOut;
    uint _padding[3];
};
struct ColorGradingSettings {
    float exposure;
    float saturation;
    float contrast;
    float hueShift;
};
struct ToneMappingSettings {
    int type;
    float gamma;
    uint _padding[2];
    LottesSettings lottesSettings;
};

/**
 * @ref https://en.wikipedia.org/wiki/Relative_luminance
 */
INLINE float Luminance(IN(vec3) a_LinearColor)
{
    return dot(a_LinearColor, vec3(0.2125, 0.7154, 0.0721));
}

INLINE vec3 SRGBToLinear(IN(vec3) a_Color, IN(float) a_Gamma)
{
    return pow(a_Color, vec3(a_Gamma));
}

INLINE vec3 SRGBToLinear(IN(vec3) a_Color)
{
    return SRGBToLinear(a_Color, DEFAULT_GAMMA);
}

INLINE vec3 LinearToSRGB(IN(vec3) a_Color, IN(float) a_Gamma)
{
    return pow(a_Color, vec3(1.f / a_Gamma));
}

INLINE vec3 LinearToSRGB(IN(vec3) a_Color)
{
    return LinearToSRGB(a_Color, DEFAULT_GAMMA);
}

INLINE float LinearToRec709(IN(float) a_Linear, IN(float) a_Gamma)
{
    if (a_Linear < 0.018)
        return 4.5 * a_Linear;
    else
        return 1.099 * pow(a_Linear, 1.f / a_Gamma) - 0.099;
}

INLINE vec3 LinearToRec709(IN(vec3) a_LinearColor, IN(float) a_Gamma)
{
    return vec3(
        LinearToRec709(a_LinearColor.r, a_Gamma),
        LinearToRec709(a_LinearColor.g, a_Gamma),
        LinearToRec709(a_LinearColor.b, a_Gamma));
}

INLINE vec3 LinearToRec709(IN(vec3) a_LinearColor)
{
    return LinearToRec709(a_LinearColor, DEFAULT_GAMMA);
}

INLINE float Rec709ToLinear(IN(float) a_Rec709, IN(float) a_Gamma)
{
    if (a_Rec709 < 0.081)
        return a_Rec709 / 4.5;
    else
        return pow((a_Rec709 + 0.099) / 1.099, a_Gamma);
}

INLINE vec3 Rec709ToLinear(IN(vec3) a_Rec709Color, IN(float) a_Gamma)
{
    return vec3(
        Rec709ToLinear(a_Rec709Color.r, a_Gamma),
        Rec709ToLinear(a_Rec709Color.g, a_Gamma),
        Rec709ToLinear(a_Rec709Color.b, a_Gamma));
}

INLINE vec3 Rec709ToLinear(IN(vec3) a_Rec709Color)
{
    return Rec709ToLinear(a_Rec709Color, DEFAULT_GAMMA);
}

INLINE vec3 ToneMappingPBRNeutral(vec3 a_Color)
{
    const float startCompression = 0.8 - 0.04;
    const float desaturation     = 0.15;

    float x      = min(a_Color.r, min(a_Color.g, a_Color.b));
    float offset = x < 0.08 ? x - 6.25 * x * x : 0.04;
    a_Color      = a_Color - offset;

    float peak = max(a_Color.r, max(a_Color.g, a_Color.b));
    if (peak < startCompression)
        return a_Color;

    const float d = 1. - startCompression;
    float newPeak = 1. - d * d / (peak + d - startCompression);
    a_Color       = a_Color * (newPeak / peak);

    float g = 1. - 1. / (desaturation * (peak - newPeak) + 1.);
    return mix(a_Color, newPeak * vec3(1, 1, 1), g);
}

// Lottes 2016, "Advanced Techniques and Optimization of HDR Color Pipelines" (AMD)
// https://github.com/GPUOpen-LibrariesAndSDKs/FidelityFX-SDK/blob/d7531ae47d8b36a5d4025663e731a47a38be882f/framework/cauldron/framework/inc/shaders/tonemapping/tonemappers.hlsl#L21
INLINE vec3 ToneMappingLottes(vec3 a_Color, const float a_Contrast, const float a_Shoulder, const float a_HdrMax, const float a_MidIn, const float a_MidOut)
{
    const float contrastTimesShoulder = a_Contrast * a_Shoulder;
    const float b                     = -((-pow(a_MidIn, a_Contrast) + (a_MidOut * (pow(a_HdrMax, contrastTimesShoulder) * pow(a_MidIn, a_Contrast) - pow(a_HdrMax, a_Contrast) * pow(a_MidIn, contrastTimesShoulder) * a_MidOut)) / (pow(a_HdrMax, contrastTimesShoulder) * a_MidOut - pow(a_MidIn, contrastTimesShoulder) * a_MidOut))
        / (pow(a_MidIn, contrastTimesShoulder) * a_MidOut));
    const float c                     = (pow(a_HdrMax, contrastTimesShoulder) * pow(a_MidIn, a_Contrast) - pow(a_HdrMax, a_Contrast) * pow(a_MidIn, contrastTimesShoulder) * a_MidOut)
        / (pow(a_HdrMax, contrastTimesShoulder) * a_MidOut - pow(a_MidIn, contrastTimesShoulder) * a_MidOut);

    a_Color    = min(a_Color, vec3(a_HdrMax)); // fix for a_Shoulder > 1
    float peak = max(a_Color.r, max(a_Color.g, a_Color.b));

    peak = max(EPSILON, peak);

    vec3 ratio    = a_Color / peak;
    const float z = pow(peak, a_Contrast);
    peak          = z / (pow(z, a_Shoulder) * b + c);

    const float crosstalk       = 4.0; // controls amount of channel crosstalk
    const float saturation      = a_Contrast; // full tonal range saturation control
    const float crossSaturation = a_Contrast * 16.0;

    float white = 1.0;

    ratio = pow(abs(ratio), vec3(saturation / crossSaturation));
    ratio = mix(ratio, vec3(white), pow(peak, crosstalk));
    ratio = pow(abs(ratio), vec3(crossSaturation));

    return peak * ratio;
}

INLINE vec3 ToneMappingLottes(IN(vec3) a_Color)
{
    const float hdrMax   = 100.0;
    const float contrast = 1.0;
    const float shoulder = 1.0;
    const float midIn    = 0.18;
    const float midOut   = 0.18;
    return ToneMappingLottes(a_Color, contrast, shoulder, hdrMax, midIn, midOut);
}

INLINE vec3 ToneMappingACES(IN(vec3) a_LinearColor)
{
    // sRGB => XYZ => D65_2_D60 => AP1 => RRT_SAT
    const mat3x3 inMat = mat3x3(
        0.59719, 0.07600, 0.02840,
        0.35458, 0.90834, 0.13383,
        0.04823, 0.01566, 0.83777);
    // ODT_SAT => XYZ => D60_2_D65 => sRGB
    const mat3x3 outMat = mat3x3(
        1.60475, -0.10208, -0.00327,
        -0.53108, 1.10813, -0.07276,
        -0.07367, -0.00605, 1.07602);

    vec3 col     = inMat * LinearToRec709(a_LinearColor);
    const vec3 a = col * (col + 0.0245786f) - 0.000090537f;
    const vec3 b = col * (0.983729f * col + 0.4329510f) + 0.238081f;
    col          = a / b;
    return Rec709ToLinear(outMat * col);
}

INLINE vec3 ToneMappingReinhard(IN(vec3) a_Color)
{
    return a_Color / (a_Color + 1.f);
}

/**
 * @brief optimized hue shift from vmedea's comment on some github gist
 * @see https://gist.github.com/mairod/a75e7b44f68110e1576d77419d608786?permalink_comment_id=4438484#gistcomment-4438484
 */
INLINE vec3 HueShift(IN(vec3) a_Color, IN(float) a_HueShift)
{
    const float s    = sin(a_HueShift);
    const float c    = cos(a_HueShift);
    const mat3x3 mat = mat3x3(
        vec3(0.167444, 0.329213, -0.496657),
        vec3(-0.327948, 0.035669, 0.292279),
        vec3(1.250268, -1.047561, -0.202707));
    return (a_Color * c) + (a_Color * s) * mat + dot(vec3(0.299, 0.587, 0.114), a_Color) * (1.0f - c);
}

INLINE vec3 Saturation(IN(vec3) a_Color, IN(float) a_Adjustment)
{
    const vec3 grayscale = vec3(Luminance(a_Color));
    return max(vec3(0.f), mix(grayscale, a_Color, 1.0 + a_Adjustment));
}

INLINE vec3 Contrast(IN(vec3) a_Color, IN(float) a_Adjustment)
{
    return max(vec3(0.f), 0.5f + (1.0f + a_Adjustment) * (a_Color - 0.5f));
}

INLINE vec3 Exposure(IN(vec3) a_Color, IN(float) a_Adjustment)
{
    return max(vec3(0.f), (1.0f + a_Adjustment) * a_Color);
}

INLINE vec3 Gamma(IN(vec3) a_Color, IN(float) a_Gamma)
{
    return LinearToRec709(a_Color, a_Gamma);
}

INLINE vec3 ApplyToneMapping(IN(vec3) a_Color, IN(ToneMappingSettings) a_ToneMapping)
{
    vec3 outColor = a_Color;
    if (a_ToneMapping.type == TONEMAP_ACES)
        outColor = ToneMappingACES(outColor);
    else if (a_ToneMapping.type == TONEMAP_REINHARD)
        outColor = ToneMappingReinhard(outColor);
    else if (a_ToneMapping.type == TONEMAP_LOTTES) {
        outColor = ToneMappingLottes(outColor,
            a_ToneMapping.lottesSettings.contrast,
            a_ToneMapping.lottesSettings.shoulder,
            a_ToneMapping.lottesSettings.hdrMax,
            a_ToneMapping.lottesSettings.midIn,
            a_ToneMapping.lottesSettings.midOut);
    } else if (a_ToneMapping.type == TONEMAP_NEUTRAL)
        outColor = ToneMappingLottes(outColor);
    return LinearToRec709(outColor, a_ToneMapping.gamma);
}

INLINE vec3 ApplyColorGrading(IN(vec3) a_Color, IN(ColorGradingSettings) a_ColorGrading)
{
    vec3 outColor = a_Color;
    outColor      = Exposure(outColor, a_ColorGrading.exposure);
    outColor      = Saturation(outColor, a_ColorGrading.saturation);
    outColor      = Contrast(outColor, a_ColorGrading.contrast);
    outColor      = HueShift(outColor, a_ColorGrading.hueShift);
    return outColor;
}

#ifdef __cplusplus
}
#endif //__cplusplus
#endif // TONEMAPPING_GLSL