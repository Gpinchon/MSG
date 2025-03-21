#include <FogCulling.glsl>
#include <Random.glsl>
#include <Bindings.glsl>
#include <FrameInfo.glsl>

layout(binding = 0) uniform sampler3D u_FogColor;
layout(binding = 1) uniform sampler2D u_Depth;
layout(binding = UBO_FRAME_INFO) uniform FrameInfoBlock
{
    FrameInfo u_FrameInfo;
};

layout(location = 0) in vec2 in_UV;

layout(location = 0) out vec4 out_Color;

void main()
{
    const float backDepth = texture(u_Depth, in_UV).r;
    const float stepSize =  1 / float(FOG_STEPS);
    const vec3 noise = ((Rand3DPCG16(ivec3(gl_FragCoord.xy, u_FrameInfo.frameIndex)) / vec3(0xffff)) * 2.f - 1.f) * 0.005f;
    vec4 result = vec4(0);
    //vec4 result = vec4(1);
    for (vec3 uv = vec3(in_UV, 0) + noise; uv.z < 1 && uv.z < backDepth; uv.z += stepSize) {
        vec4 fogColor = texture(u_FogColor, uv);
        result += fogColor * stepSize;
		//result *= exp(-fogColor * stepSize);
    }
    out_Color = result;
    //out_Color = clamp(1 - result, vec4(0), vec4(1));
}
