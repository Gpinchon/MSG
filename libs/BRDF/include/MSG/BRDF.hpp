#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////
namespace Msg {
class Texture;
class Image;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace Msg::BRDF {
enum class Type {
    Standard,
    Sheen
};
/**
 * @param a_Xi random vector (usually Halton23 but could also be Hammersley)
 * @param a_N normal vector
 * @param a_Roughness perceptial roughness
 * @return glm::vec3
 */
glm::vec3 ImportanceSampleGGX(const glm::vec2& a_Xi, const glm::vec3& a_N, const float& a_Roughness);
glm::vec2 IntegrateBRDF(const float& roughness, const float& NdotV, const Type& a_Type);
Image GenerateImage(const Type& a_Type, const uint32_t& a_Width = 256, const uint32_t& a_Height = 256);
Texture GenerateTexture(const Type& a_Type, const uint32_t& a_Width = 256, const uint32_t& a_Height = 256);
}