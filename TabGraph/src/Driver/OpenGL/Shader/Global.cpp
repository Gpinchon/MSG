/*
* @Author: gpinchon
* @Date:   2021-04-13 22:39:30
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-13 22:51:30
*/

#include <map>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <memory>

namespace TabGraph::Textures {
class Texture;
}

namespace TabGraph::Shader::Global::Impl {
static std::map<std::string, std::string> s_globalDefines;
static std::map<std::string, std::vector<float>> s_globalFloats;
static std::map<std::string, std::vector<glm::vec2>> s_globalVec2;
static std::map<std::string, std::vector<glm::vec3>> s_globalVec3;
static std::map<std::string, std::vector<glm::vec4>> s_globalVec4;
static std::map<std::string, std::vector<glm::mat4>> s_globalMat4;
static std::map<std::string, std::vector<glm::mat3>> s_globalMat3;
static std::map<std::string, std::vector<int32_t>> s_globalInts;
static std::map<std::string, std::vector<uint32_t>> s_globalUints;
static std::map<std::string, std::shared_ptr<Textures::Texture>> s_globalTextures;
void SetTexture(const std::string& name, const std::shared_ptr<Textures::Texture> value)
{
    s_globalTextures[name] = value;
}
void SetUniform(const std::string& name, const float* value, const uint16_t count, const uint16_t index)
{
    auto& values { s_globalFloats[name] };
    if (values.size() <= index + count)
        values.resize(index + count, 0);
    std::copy(value, value + count, values.begin() + index);
}
void SetUniform(const std::string& name, const float value)
{
    SetUniform(name, &value, 1, 0);
}
void SetUniform(const std::string& name, const int32_t* value, const uint16_t count, const uint16_t index)
{
    auto& values { s_globalInts[name] };
    if (values.size() <= index + count)
        values.resize(index + count, 0);
    std::copy(value, value + count, values.begin() + index);
}
void SetUniform(const std::string& name, const int32_t value)
{
    SetUniform(name, &value, 1, 0);
}
void SetUniform(const std::string& name, const uint32_t* value, const uint16_t count, const uint16_t index)
{
    auto& values { s_globalUints[name] };
    if (values.size() <= index + count)
        values.resize(index + count, 0);
    std::copy(value, value + count, values.begin() + index);
}
void SetUniform(const std::string& name, const uint32_t value)
{
    SetUniform(name, &value, 1, 0);
}
void SetUniform(const std::string& name, const glm::vec2* value, const uint16_t count, const uint16_t index)
{
    auto& values { s_globalVec2[name] };
    if (values.size() <= index + count)
        values.resize(index + count, glm::vec2(0));
    std::copy(value, value + count, values.begin() + index);
}
void SetUniform(const std::string& name, const glm::vec2& value)
{
    SetUniform(name, &value, 1, 0);
}
void SetUniform(const std::string& name, const glm::vec3* value, const uint16_t count, const uint16_t index)
{
    auto& values { s_globalVec3[name] };
    if (values.size() <= index + count)
        values.resize(index + count, glm::vec3(0));
    std::copy(value, value + count, values.begin() + index);
}
void SetUniform(const std::string& name, const glm::vec3& value)
{
    SetUniform(name, &value, 1, 0);
}
void SetUniform(const std::string& name, const glm::vec4* value, const uint16_t count, const uint16_t index)
{
    auto& values { s_globalVec4[name] };
    if (values.size() <= index + count)
        values.resize(index + count, glm::vec4(0));
    std::copy(value, value + count, values.begin() + index);
}
void SetUniform(const std::string& name, const glm::vec4& value)
{
    SetUniform(name, &value, 1, 0);
}
void SetUniform(const std::string& name, const glm::mat4* value, const uint16_t count, const uint16_t index)
{
    auto& values { s_globalMat4[name] };
    if (values.size() <= index + count)
        values.resize(index + count, glm::mat4(0));
    std::copy(value, value + count, values.begin() + index);
}
void SetUniform(const std::string& name, const glm::mat4& value)
{
    SetUniform(name, &value, 1, 0);
}
void SetUniform(const std::string& name, const glm::mat3* value, const uint16_t count, const uint16_t index)
{
    auto& values { s_globalMat3[name] };
    if (values.size() <= index + count)
        values.resize(index + count, glm::mat4(0));
    std::copy(value, value + count, values.begin() + index);
}
void SetUniform(const std::string& name, const glm::mat3& value)
{
    SetUniform(name, &value, 1, 0);
}
void SetDefine(const std::string& name, const std::string& value)
{
    s_globalDefines[name] = value;
}
void RemoveDefine(const std::string& name)
{
    s_globalDefines.erase(name);
}
std::map<std::string, std::string>& GetDefines()
{
    return s_globalDefines;
}
std::map<std::string, std::vector<float>>& GetFloats()
{
    return s_globalFloats;
}
std::map<std::string, std::vector<glm::vec2>>& GetVec2()
{
    return s_globalVec2;
}
std::map<std::string, std::vector<glm::vec3>>& GetVec3()
{
    return s_globalVec3;
}
std::map<std::string, std::vector<glm::vec4>>& GetVec4()
{
    return s_globalVec4;
}
std::map<std::string, std::vector<glm::mat4>>& GetMat4()
{
    return s_globalMat4;
}
std::map<std::string, std::vector<glm::mat3>>& GetMat3()
{
    return s_globalMat3;
}
std::map<std::string, std::vector<int32_t>>& GetInts()
{
    return s_globalInts;
}
std::map<std::string, std::vector<uint32_t>>& GetUints()
{
    return s_globalUints;
}
std::map<std::string, std::shared_ptr<Textures::Texture>>& GetTextures()
{
    return s_globalTextures;
}
}
