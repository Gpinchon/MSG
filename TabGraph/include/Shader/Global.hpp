/*
* @Author: gpinchon
* @Date:   2021-02-22 11:08:16
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-13 22:39:13
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <glm/fwd.hpp>
#include <memory>
#include <string>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Textures {
class Texture;
}

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Shader::Global {
void SetTexture(const std::string& name, const std::shared_ptr<Textures::Texture> value);
void SetUniform(const std::string& name, const float value);
void SetUniform(const std::string& name, const float* value, const uint16_t count = 1, const uint16_t index = 0);
void SetUniform(const std::string& name, const int32_t value);
void SetUniform(const std::string& name, const int32_t* value, const uint16_t count = 1, const uint16_t index = 0);
void SetUniform(const std::string& name, const uint32_t value);
void SetUniform(const std::string& name, const uint32_t* value, const uint16_t count = 1, const uint16_t index = 0);
void SetUniform(const std::string& name, const glm::vec2& value);
void SetUniform(const std::string& name, const glm::vec2* value, const uint16_t count, const uint16_t index);
void SetUniform(const std::string& name, const glm::vec3& value);
void SetUniform(const std::string& name, const glm::vec3* value, const uint16_t count, const uint16_t index);
void SetUniform(const std::string& name, const glm::vec4& value);
void SetUniform(const std::string& name, const glm::vec4* value, const uint16_t count, const uint16_t index);
void SetUniform(const std::string& name, const glm::mat4& value);
void SetUniform(const std::string& name, const glm::mat4* value, const uint16_t count, const uint16_t index);
void SetUniform(const std::string& name, const glm::mat3& value);
void SetUniform(const std::string& name, const glm::mat3* value, const uint16_t count, const uint16_t index);
void SetDefine(const std::string& name, const std::string& value);
void RemoveDefine(const std::string& name);
}