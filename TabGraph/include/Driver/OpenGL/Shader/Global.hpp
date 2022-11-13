/*
* @Author: gpinchon
* @Date:   2021-04-13 22:36:16
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-13 22:50:26
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <glm/fwd.hpp>
#include <memory>
#include <string>
#include <map>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Textures {
class Texture;
}

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Shader::Global::Impl {
void SetTexture(const std::string& name, const std::shared_ptr<Textures::Texture> value);
void SetUniform(const std::string& name, const float value);
void SetUniform(const std::string& name, const float* value, const uint16_t count, const uint16_t index);
void SetUniform(const std::string& name, const int32_t value);
void SetUniform(const std::string& name, const int32_t* value, const uint16_t count, const uint16_t index);
void SetUniform(const std::string& name, const uint32_t value);
void SetUniform(const std::string& name, const uint32_t* value, const uint16_t count, const uint16_t index);
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
std::map<std::string, std::string>& GetDefines();
std::map<std::string, std::vector<float>>& GetFloats();
std::map<std::string, std::vector<glm::vec2>>& GetVec2();
std::map<std::string, std::vector<glm::vec3>>& GetVec3();
std::map<std::string, std::vector<glm::vec4>>& GetVec4();
std::map<std::string, std::vector<glm::mat4>>& GetMat4();
std::map<std::string, std::vector<glm::mat3>>& GetMat3();
std::map<std::string, std::vector<int32_t>>& GetInts();
std::map<std::string, std::vector<uint32_t>>& GetUints();
std::map<std::string, std::shared_ptr<Textures::Texture>>& GetTextures();
}