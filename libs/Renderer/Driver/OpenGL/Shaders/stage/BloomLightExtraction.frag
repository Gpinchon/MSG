#include <Bloom.glsl>
#include <KarisAverage.glsl>

layout(binding = 0) uniform sampler2D u_Source;
layout(binding = 0) uniform BloomSettingsBlock
{
    BloomSettings u_Settings;
};

layout(location = 0) in vec2 in_UV;
layout(location = 0) out vec4 out_Color;

/**
 * @brief extract pixel color using luminance.
 * Strongly inspired by pmndrs postprocess repo (see luminance.frag file) because it works quite well !
 *
 */
void main()
{
    vec4 color            = KarisAverageSample(u_Source, in_UV);
    const float edge0     = u_Settings.threshold;
    const float edge1     = u_Settings.threshold + u_Settings.smoothing;
    const float luminance = Luminance(color.rgb);
    // apply a high pass filter using luminance to avoid bloom "overload"
    const float mask = smoothstep(edge0, edge1, luminance);
    out_Color        = min(vec4(1000), vec4(color.rgb * u_Settings.tint * mask, 1));
}
