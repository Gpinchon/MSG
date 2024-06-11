#ifndef GLSL_TYPES
#define GLSL_TYPES

#ifdef __cplusplus
#include <glm/glm.hpp>
#define VEC2      glm::vec2
#define VEC3      glm::vec3
#define VEC4      glm::vec4
#define MAT2x2    glm::mat2x2
#define MAT3x3    glm::mat3x3
#define MAT4x4    glm::mat4x4
#define SAMPLER2D uint64_t
#define UINT      uint32_t
#else
#define VEC2      vec2
#define VEC3      vec3
#define VEC4      vec4
#define MAT2x2    mat2x2
#define MAT3x3    mat3x3
#define MAT4x4    mat4x4
#define SAMPLER2D sampler2D
#define UINT      uint
#endif //__cplusplus

#endif // GLSL_TYPES
