/*
* @Author: gpinchon
* @Date:   2021-04-13 23:08:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-13 23:22:56
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Shader/Program.hpp>
#include <Shader/Stage.hpp>
#include <Driver/OpenGL/ObjectHandle.hpp>

#include <array>
#include <glm/fwd.hpp>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Textures {
class Texture;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Shader {
class Program::Impl {
public:
    using Handle = OpenGL::ObjectHandle;
    READONLYPROPERTY(bool, Compiled, false);
    PROPERTY(std::string, GLSLVersion, "440");

public:
    ~Impl();
    const Handle& GetHandle() const;
    void Attach(const Stage& stage);
    Stage& GetStage(Stage::Type);
    void Compile();
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
    void Use();
    static void UseNone();

private:
    void _SetHandle(uint32_t value);
    std::array<Stage, (int)Stage::Type::MaxValue> _stages;
    std::map<std::string, std::string> _defines;
    std::map<std::string, int32_t> _uniformLocs;
    std::map<int32_t, int32_t> _textureIndice;
    Handle _handle { 0 };
};
};