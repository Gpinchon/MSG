#ifndef FUNCTIONS_GLSL
#define FUNCTIONS_GLSL

#ifdef __cplusplus

#include <Types.glsl>
#include <glm/common.hpp>
#include <glm/integer.hpp>

namespace TabGraph::Renderer::GLSL {
constexpr auto fract           = glm::fract;
constexpr auto all             = glm::all;
constexpr auto dot             = glm::dot;
constexpr auto bitfieldReverse = glm::bitfieldReverse;
constexpr auto lessThanEqual   = glm::lessThanEqual;
}

#define IN(type)               const type&
#define OUT(type)              type&
#define INOUT(type)            type&
#define M_PI                   3.1415926535897932384626433832795
#define PI                     M_PI
#define EPSILON                0.0001
#define lequal(a, b)           all(lessThanEqual(a, b))
#define Luminance(linearColor) dot(linearColor, vec3(0.299, 0.587, 0.114))

#else //__cplusplus

#define IN(type)    in type
#define OUT(type)   out type
#define INOUT(type) inout type

#endif //__cplusplus

#endif // FUNCTIONS_GLSL
