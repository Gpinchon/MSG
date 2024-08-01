#ifndef SPHERICAL_HARMONICS_GLSL
#define SPHERICAL_HARMONICS_GLSL

#include <Functions.glsl>

#define SH_SAMPLES_COUNT 16

vec3 SampleSH(IN(vec4) a_SH[SH_SAMPLES_COUNT], IN(vec3) a_N)
{
    const vec3 N2 = a_N * a_N;
    vec3 v        = vec3(0);
    v += a_SH[0].xyz * 0.282095;

    v += a_SH[1].xyz * -0.488603 * a_N.y;
    v += a_SH[2].xyz * 0.488603 * a_N.z;
    v += a_SH[3].xyz * -0.488603 * a_N.x;

    v += a_SH[4].xyz * 1.092548 * a_N.x * a_N.y;
    v += a_SH[5].xyz * -1.092548 * a_N.y * a_N.z;
    v += a_SH[6].xyz * 0.315392 * (-N2.x - N2.y + 2.0 * N2.z);
    v += a_SH[7].xyz * -1.092548 * a_N.x * a_N.z;
    v += a_SH[8].xyz * 0.546274 * (N2.x - N2.y);

    v += a_SH[9].xyz * -0.590044 * a_N.y * (3 * N2.x - N2.y);
    v += a_SH[10].xyz * 2.890611 * a_N.x * a_N.y * a_N.z;
    v += a_SH[11].xyz * -0.457046 * a_N.y * (4 * N2.z - N2.x - N2.y);
    v += a_SH[12].xyz * 0.373176 * a_N.z * (2 * N2.z - 3 * N2.x - 3 * N2.y);
    v += a_SH[13].xyz * -0.457046 * a_N.x * (4 * N2.z - N2.x - N2.y);
    v += a_SH[14].xyz * 1.445306 * a_N.z * (N2.x - N2.y);
    v += a_SH[15].xyz * -0.590044 * a_N.x * (N2.x - 3 * N2.y);
    // for (int i = 0; i < SHSAMPLES; ++i)
    //     v += max(vec3(0), dot(a_N, SHCartesianShapes[i]) * SH[i]);
    return max(vec3(0), v);
}

#endif // SPHERICAL_HARMONICS_GLSL