/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-13 13:10:05
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Core/Property.hpp>
#include <Core/Inherit.hpp>
#include <Core/Object.hpp>
#include <Shader/Stage.hpp>

#include <glm/fwd.hpp>
#include <string>
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
class Program : public Core::Inherit<Core::Object, Program> {
    class Impl;
    friend Impl;

public:
    Program();
    Program(const Program& other);
    Program(const std::string& name);
    ~Program();
    Stage& GetStage(Stage::Type);
    Program& Compile();
    Program& Attach(const Stage& stage);
    Program& Use();
    Program& SetTexture(const std::string& name, const std::shared_ptr<Textures::Texture> value);
    Program& SetUniform(const std::string& name, const float value);
    Program& SetUniform(const std::string& name, const float* value, const uint16_t count = 1, const uint16_t index = 0);
    Program& SetUniform(const std::string& name, const int32_t value);
    Program& SetUniform(const std::string& name, const int32_t* value, const uint16_t count = 1, const uint16_t index = 0);
    Program& SetUniform(const std::string& name, const uint32_t value);
    Program& SetUniform(const std::string& name, const uint32_t* value, const uint16_t count = 1, const uint16_t index = 0);
    Program& SetUniform(const std::string& name, const glm::vec2& value);
    Program& SetUniform(const std::string& name, const glm::vec2* value, const uint16_t count = 1, const uint16_t index = 0);
    Program& SetUniform(const std::string& name, const glm::vec3& value);
    Program& SetUniform(const std::string& name, const glm::vec3* value, const uint16_t count = 1, const uint16_t index = 0);
    Program& SetUniform(const std::string& name, const glm::vec4& value);
    Program& SetUniform(const std::string& name, const glm::vec4* value, const uint16_t count = 1, const uint16_t index = 0);
    Program& SetUniform(const std::string& name, const glm::mat4& value);
    Program& SetUniform(const std::string& name, const glm::mat4* value, const uint16_t count = 1, const uint16_t index = 0);
    Program& SetUniform(const std::string& name, const glm::mat3& value);
    Program& SetUniform(const std::string& name, const glm::mat3* value, const uint16_t count = 1, const uint16_t index = 0);
    Program& SetDefine(const std::string& name, const std::string& value = "");
    Program& RemoveDefine(const std::string& name);
    inline void Done() {
        UseNone();
    }
    bool GetCompiled() const;
    static void UseNone();

private:
    std::shared_ptr<Impl> _impl;
};
};
