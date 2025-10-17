#ifndef FUNCTIONS_GLSL
#define FUNCTIONS_GLSL

#ifdef __cplusplus

#include <Types.glsl>
#include <glm/common.hpp>
#include <glm/integer.hpp>

namespace Msg::Renderer::GLSL {
template <typename T>
constexpr auto fract = glm::fract<T>;
template <typename T>
constexpr auto all = glm::all<T>;
template <typename T>
constexpr auto bitfieldReverse = glm::bitfieldReverse<T>;
template <typename T>
constexpr auto lessThanEqual = glm::lessThanEqual<T>;
template <typename T>
auto isinf(const T& a_V) { return glm::isinf(a_V); };
template <typename T>
auto normalize(const T& a_V) { return glm::normalize(a_V); }
template <typename T>
float distance(const T& a_X, const T& a_Y) { return glm::distance(a_X, a_Y); }
template <typename T>
T clamp(const T& a_X, const T& a_Y, const T& a_Z) { return glm::clamp(a_X, a_Y, a_Z); }
template <typename T>
float dot(const T& a_X, const T& a_Y) { return glm::dot(a_X, a_Y); }
template <typename T>
T max(const T& a_X, const T& a_Y) { return glm::max(a_X, a_Y); }
template <typename T>
T min(const T& a_X, const T& a_Y) { return glm::min(a_X, a_Y); }
inline float compMax(const vec2& v) { return max(v.x, v.y); }
inline float compMax(const vec3& v) { return max(compMax(vec2(v.x, v.y)), v.z); }
inline float compMax(const vec4& v) { return max(compMax(vec3(v.x, v.y, v.z)), v.w); }
inline float compMin(const vec2& v) { return min(v.x, v.y); }
inline float compMin(const vec3& v) { return min(compMin(vec2(v.x, v.y)), v.z); }
inline float compMin(const vec4& v) { return min(compMin(vec3(v.x, v.y, v.z)), v.w); }
}

#define IN(type)    const type&
#define OUT(type)   type&
#define INOUT(type) type&
#define INLINE      inline
#define xy          xy()
#define xyz         xyz()
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif // M_PI
#define PI                     M_PI
#define EPSILON                0.00001
#define lequal(a, b)           all(lessThanEqual(a, b))
#define Luminance(linearColor) dot(linearColor, vec3(0.299, 0.587, 0.114))
#define saturate(x)            clamp(x, 0, 1)
#define MIPMAPNBR(size)        int((size.x <= 0 && size.y <= 0) ? 0 : floor(log2(GLSL::compMax(size))) + 1)

#else //__cplusplus

#define IN(type)                in type
#define OUT(type)               out type
#define INOUT(type)             inout type
#define INLINE                  /*NOTHING*/
#define M_PI                    3.1415926535897932384626433832795
#define PI                      M_PI
#define EPSILON                 0.00001
#define saturate(x)             clamp(x, 0, 1)
#define MIPMAPNBR(size)         int((size.x <= 0 && size.y <= 0) ? 0 : floor(log2(compMax(size))) + 1)
#define sampleLod(tex, uv, lod) textureLod(tex, uv, lod* textureQueryLevels(tex))
float compMax(IN(vec2) v) { return max(v.x, v.y); }
float compMax(IN(vec3) v) { return max(compMax(v.xy), v.z); }
float compMax(IN(vec4) v) { return max(compMax(v.xyz), v.w); }
float compMin(IN(vec2) v) { return min(v.x, v.y); }
float compMin(IN(vec3) v) { return min(compMin(vec2(v.xy)), v.z); }
float compMin(IN(vec4) v) { return min(compMin(vec3(v.xyz)), v.w); }
float remap(
    IN(float) a_Val,
    IN(float) a_Min,
    IN(float) a_Max,
    IN(float) a_NewMin,
    IN(float) a_NewMax)
{
    float clampedVal = clamp(a_Val, a_Min, a_Max);
    return ((clampedVal - a_Min) / (a_Max - a_Min)) * (a_NewMax - a_NewMin) + a_NewMin;
}

/**
 * @brief normalizes a value between [a_Min, a_Max] to [0, 1]
 *
 * @return float
 */
float normalizeValue(IN(float) a_Val, IN(float) a_Min, IN(float) a_Max)
{
    return remap(a_Val, a_Min, a_Max, 0.f, 1.f);
}

#endif //__cplusplus

#endif // FUNCTIONS_GLSL
