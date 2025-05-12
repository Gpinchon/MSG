#ifndef BICUBIC_GLSL
#define BICUBIC_GLSL
#include <Functions.glsl>

vec4 textureNice(sampler2DArray sam, vec3 uv)
{
    vec2 textureResolution = vec2(textureSize(sam, 0).xy);
    uv.xy                  = uv.xy * textureResolution + 0.5;
    vec2 iuv               = floor(uv.xy);
    vec2 fuv               = fract(uv.xy);
    uv.xy                  = iuv + fuv * fuv * (3.0 - 2.0 * fuv);
    uv.xy                  = (uv.xy - 0.5) / textureResolution;
    return texture(sam, uv);
}

/**
 * @brief this tries to be a cheaper alternative to real bucibuc sampling
 * @see https://www.shadertoy.com/view/XsfGDn
 */
vec4 textureNice(sampler2D sam, vec2 uv)
{
    vec2 textureResolution = vec2(textureSize(sam, 0));
    uv                     = uv * textureResolution + 0.5;
    vec2 iuv               = floor(uv);
    vec2 fuv               = fract(uv);
    uv                     = iuv + fuv * fuv * (3.0 - 2.0 * fuv);
    uv                     = (uv - 0.5) / textureResolution;
    return texture(sam, uv);
}

/**
 * @brief this tries to be a cheaper alternative to real bucibuc sampling
 * @see https://www.shadertoy.com/view/XsfGDn
 */
vec4 textureNice(sampler3D sam, vec3 uv)
{
    vec3 textureResolution = vec3(textureSize(sam, 0));
    uv                     = uv * textureResolution + 0.5;
    vec3 iuv               = floor(uv);
    vec3 fuv               = fract(uv);
    uv                     = iuv + fuv * fuv * (3.0 - 2.0 * fuv);
    uv                     = (uv - 0.5) / textureResolution;
    return texture(sam, uv);
}

vec4 cubic(IN(float) v)
{
    vec4 n  = vec4(1.0, 2.0, 3.0, 4.0) - v;
    vec4 s  = n * n * n;
    float x = s.x;
    float y = s.y - 4.0 * s.x;
    float z = s.z - 4.0 * s.y + 6.0 * s.x;
    float w = 6.0 - x - y - z;
    return vec4(x, y, z, w) / 6.f;
}

vec4 textureBicubic(IN(sampler2D) a_Sampler, vec2 a_UVW)
{
    vec2 texSize    = textureSize(a_Sampler, 0);
    vec2 invTexSize = 1.0 / texSize;

    a_UVW = a_UVW * texSize - 0.5;

    vec2 fxy = fract(a_UVW);
    a_UVW -= fxy;

    vec4 xcubic = cubic(fxy.x);
    vec4 ycubic = cubic(fxy.y);

    vec4 c = a_UVW.xxyy + vec2(-0.5, +1.5).xyxy;

    vec4 s      = vec4(xcubic.xz + xcubic.yw, ycubic.xz + ycubic.yw);
    vec4 offset = c + vec4(xcubic.yw, ycubic.yw) / s;

    offset *= invTexSize.xxyy;

    vec4 sample0 = texture(a_Sampler, offset.xz);
    vec4 sample1 = texture(a_Sampler, offset.yz);
    vec4 sample2 = texture(a_Sampler, offset.xw);
    vec4 sample3 = texture(a_Sampler, offset.yw);

    float sx = s.x / (s.x + s.y);
    float sy = s.z / (s.z + s.w);

    return mix(
        mix(sample3, sample2, sx),
        mix(sample1, sample0, sx),
        sy);
}

vec4 textureBicubic(IN(sampler3D) a_Sampler, vec3 a_UVW)
{
    vec3 texSize   = textureSize(a_Sampler, 0);
    vec4 texelSize = vec4(1.0 / texSize.xz, texSize.xz);

    a_UVW = a_UVW * texSize - 0.5;

    vec3 f = fract(a_UVW);
    a_UVW -= f;

    vec4 xcubic = cubic(f.x);
    vec4 ycubic = cubic(f.y);
    vec4 zcubic = cubic(f.z);

    vec2 cx      = a_UVW.xx + vec2(-0.5, 1.5);
    vec2 cy      = a_UVW.yy + vec2(-0.5, 1.5);
    vec2 cz      = a_UVW.zz + vec2(-0.5, 1.5);
    vec2 sx      = xcubic.xz + xcubic.yw;
    vec2 sy      = ycubic.xz + ycubic.yw;
    vec2 sz      = zcubic.xz + zcubic.yw;
    vec2 offsetx = cx + xcubic.yw / sx;
    vec2 offsety = cy + ycubic.yw / sy;
    vec2 offsetz = cz + zcubic.yw / sz;
    offsetx /= texSize.xx;
    offsety /= texSize.yy;
    offsetz /= texSize.zz;

    vec4 sample0 = texture(a_Sampler, vec3(offsetx.x, offsety.x, offsetz.x));
    vec4 sample1 = texture(a_Sampler, vec3(offsetx.y, offsety.x, offsetz.x));
    vec4 sample2 = texture(a_Sampler, vec3(offsetx.x, offsety.y, offsetz.x));
    vec4 sample3 = texture(a_Sampler, vec3(offsetx.y, offsety.y, offsetz.x));
    vec4 sample4 = texture(a_Sampler, vec3(offsetx.x, offsety.x, offsetz.y));
    vec4 sample5 = texture(a_Sampler, vec3(offsetx.y, offsety.x, offsetz.y));
    vec4 sample6 = texture(a_Sampler, vec3(offsetx.x, offsety.y, offsetz.y));
    vec4 sample7 = texture(a_Sampler, vec3(offsetx.y, offsety.y, offsetz.y));

    float gx = sx.x / (sx.x + sx.y);
    float gy = sy.x / (sy.x + sy.y);
    float gz = sz.x / (sz.x + sz.y);

    vec4 x0 = mix(sample1, sample0, gx);
    vec4 x1 = mix(sample3, sample2, gx);
    vec4 x2 = mix(sample5, sample4, gx);
    vec4 x3 = mix(sample7, sample6, gx);
    vec4 y0 = mix(x1, x0, gy);
    vec4 y1 = mix(x3, x2, gy);

    return mix(y1, y0, gz);
}
#endif // BICUBIC_GLSL