#include <Bloom.glsl>
#include <ToneMapping.glsl>

layout(binding = 0) uniform sampler2D u_Source;
layout(binding = 0) uniform BloomSettingsBlock
{
    BloomSettings u_Settings;
};

layout(location = 0) in vec2 in_UV;
layout(location = 0) out vec4 out_Color;

vec4 SampleSource()
{
    vec2 pixSize = 1.f / textureSize(u_Source, 0);
    vec4 offset  = pixSize.xyxy * vec4(-1, -1, 1, 1);
    vec4 col0    = texture(u_Source, in_UV + offset.xy);
    vec4 col1    = texture(u_Source, in_UV + offset.zy);
    vec4 col2    = texture(u_Source, in_UV + offset.xw);
    vec4 col3    = texture(u_Source, in_UV + offset.zw);
    float col0B  = 1.f / (Luminance(col0.rgb) + 1.f);
    float col1B  = 1.f / (Luminance(col1.rgb) + 1.f);
    float col2B  = 1.f / (Luminance(col2.rgb) + 1.f);
    float col3B  = 1.f / (Luminance(col3.rgb) + 1.f);
    float bSum   = col0B + col1B + col2B + col3B;
    return (col0 * col0B + col1 * col1B + col2 * col2B + col3 * col3B) / bSum;
}

/**
 * @brief extract pixel color using luminance.
 * Strongly inspired by pmndrs postprocess repo (see luminance.frag file) because it works quite well !
 *
 */
void main()
{
    vec4 color            = SampleSource();
    const float edge0     = u_Settings.threshold;
    const float edge1     = u_Settings.threshold + u_Settings.smoothing;
    const float luminance = Luminance(color.rgb);
    // apply a high pass filter using luminance to avoid bloom "overload"
    const float mask = smoothstep(edge0, edge1, luminance);
    out_Color        = min(vec4(1000), vec4(color.rgb * u_Settings.tint * mask, 1));
}
