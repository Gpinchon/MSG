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
float dot(const T& a_X, const T& a_Y) { return glm::dot(a_X, a_Y); }
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

#endif //__cplusplus

#endif // FUNCTIONS_GLSL
