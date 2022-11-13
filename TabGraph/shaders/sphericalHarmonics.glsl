R""(
#define SHSAMPLES 16
uniform vec3 SH[SHSAMPLES] = vec3[16](0);
//uniform vec3 SHCartesianShapes[SHSAMPLES] = vec3[16](vec3(0));

vec3   SampleSH(in vec3 N)
{
	const vec3 N2 = N * N;
    vec3 v = vec3(0);
    v += SH[0]  *  0.282095;

    v += SH[1]  * -0.488603 * N.y;
    v += SH[2]  *  0.488603 * N.z;
    v += SH[3]  * -0.488603 * N.x;

    v += SH[4]  *  1.092548 * N.x * N.y;
    v += SH[5]  * -1.092548 * N.y * N.z;
    v += SH[6]  *  0.315392 * (-N2.x - N2.y + 2.0 * N2.z);
    v += SH[7]  * -1.092548 * N.x * N.z;
    v += SH[8]  *  0.546274 * (N2.x - N2.y);

	v += SH[9]  * -0.590044 * N.y * (3 * N2.x - N2.y);
	v += SH[10] *  2.890611 * N.x * N.y * N.z;
	v += SH[11] * -0.457046 * N.y * (4 * N2.z - N2.x - N2.y);
	v += SH[12] *  0.373176 * N.z * (2 * N2.z - 3 * N2.x - 3 * N2.y);
	v += SH[13] * -0.457046 * N.x * (4 * N2.z - N2.x - N2.y);
	v += SH[14] *  1.445306 * N.z * (N2.x - N2.y);
	v += SH[15] * -0.590044 * N.x * (N2.x - 3 * N2.y);
    //for (int i = 0; i < SHSAMPLES; ++i)
    //    v += max(vec3(0), dot(N, SHCartesianShapes[i]) * SH[i]);
    return max(vec3(0), v);
}
)""