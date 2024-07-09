#ifndef FUNCTIONS_GLSL
#define FUNCTIONS_GLSL

#ifdef __cplusplus

#include <Types.glsl>
#include <glm/common.hpp>
#include <glm/integer.hpp>

namespace TabGraph::Renderer::GLSL {
template <typename T>
constexpr auto fract = glm::fract<T>;
template <typename T>
constexpr auto all = glm::all<T>;
template <typename T>
constexpr auto bitfieldReverse = glm::bitfieldReverse<T>;
template <typename T>
constexpr auto lessThanEqual = glm::lessThanEqual<T>;
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
inline float compMax(vec3 v) { return max(max(v.x, v.y), v.z); }
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
#define EPSILON                0.0001
#define lequal(a, b)           all(lessThanEqual(a, b))
#define Luminance(linearColor) dot(linearColor, vec3(0.299, 0.587, 0.114))

#else //__cplusplus

#define IN(type)    in type
#define OUT(type)   out type
#define INOUT(type) inout type
#define INLINE      /*NOTHING*/
float compMax(vec3 v) { return max(max(v.x, v.y), v.z); }

#endif //__cplusplus

#endif // FUNCTIONS_GLSL
