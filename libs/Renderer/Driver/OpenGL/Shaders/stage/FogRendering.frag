#include <FogCulling.glsl>

layout(binding = 0) uniform sampler3D u_FogColor;
layout(binding = 1) uniform sampler2D u_Color;
layout(binding = 2) uniform sampler2D u_Depth;

layout(location = 0) in vec2 in_UV;

layout(location = 0) out vec4 out_Color;

void main()
{
    const float backDepth = texture(u_Depth, in_UV).r;
    //const vec4 backColor  = texture(u_Color, in_UV);
    vec4 result = vec4(0);
    const float stepSize =  1 / float(FOG_STEPS);
    for (uint z = 0; z < FOG_STEPS; z++) {
        const vec3 uv = vec3(in_UV, z * stepSize);
        if (uv.z >= backDepth) break;
        vec4 fogColor = texture(u_FogColor, uv);
		result += fogColor * stepSize;
    }
    out_Color = result;
}
