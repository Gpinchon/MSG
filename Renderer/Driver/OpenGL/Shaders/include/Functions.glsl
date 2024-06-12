#ifndef FUNCTIONS
#define FUNCTIONS

#ifdef __cplusplus
#include <glm/common.hpp>
#include <glm/integer.hpp>
#define IN(type)    const type&
#define OUT(type)   type&
#define INOUT(type) type&
namespace TabGraph::Renderer::GLSL {
constexpr auto fract           = glm::fract;
constexpr auto bitfieldReverse = glm::bitfieldReverse
}
#else //__cplusplus
#define IN(type)    in type
#define OUT(type)   out type
#define INOUT(type) inout type
#endif //__cplusplus

#endif // FUNCTIONS
