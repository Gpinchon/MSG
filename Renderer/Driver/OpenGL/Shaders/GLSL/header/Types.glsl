#ifndef GLSL_TYPES
#define GLSL_TYPES

#ifdef __cplusplus
#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
namespace TabGraph::Renderer::GLSL {
using vec2            = glm::vec2;
using vec3            = glm::vec3;
using vec4            = glm::vec4;
using uvec2           = glm::uvec2;
using uvec3           = glm::uvec3;
using uvec4           = glm::uvec4;
using ivec2           = glm::ivec2;
using ivec3           = glm::ivec3;
using ivec4           = glm::ivec4;
using mat2x2          = glm::mat2x2;
using mat3x3          = glm::mat3x3;
using mat4x4          = glm::mat4x4;
using sampler2D       = uint64_t;
using sampler2DShadow = uint64_t;
using uint            = uint32_t;
}
#endif //__cplusplus

#endif // GLSL_TYPES
