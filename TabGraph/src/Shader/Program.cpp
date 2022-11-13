/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-20 22:55:26
*/

#include <Shader/Program.hpp>
#include <Shader/Stage.hpp>
#if RENDERINGAPI == OpenGL
#include <Driver/OpenGL/Shader/Program.hpp>
#endif

namespace TabGraph::Shader {
static auto shaderProgramNbr = 0u;
Program::Program() : Inherit("Shader_" + std::to_string(++shaderProgramNbr))
{
    _impl.reset(new Impl);
}

Program::Program(const Program& other) : Inherit(other)
{
    shaderProgramNbr++;
    _impl = other._impl;
}

Program::Program(const std::string& name) : Program()
{
    SetName(name);
}

Program::~Program()
{
}

Stage& Program::GetStage(Stage::Type type)
{
    return _impl->GetStage(type);
}

Program& Program::Compile()
{
    _impl->Compile();
    return *this;
}

Program& Program::Attach(const Stage& stage)
{
    _impl->Attach(stage);
    return *this;
}

Program& Program::Use()
{
    _impl->Use();
    return *this;
}

Program& Program::SetTexture(const std::string& name, const std::shared_ptr<Textures::Texture> value)
{
    _impl->SetTexture(name, value);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const float value)
{
    _impl->SetUniform(name, value);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const float* value, const uint16_t count, const uint16_t index)
{
    _impl->SetUniform(name, value, count, index);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const int32_t value)
{
    _impl->SetUniform(name, value);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const int32_t* value, const uint16_t count, const uint16_t index)
{
    _impl->SetUniform(name, value, count, index);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const uint32_t value)
{
    _impl->SetUniform(name, value);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const uint32_t* value, const uint16_t count, const uint16_t index)
{
    _impl->SetUniform(name, value, count, index);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const glm::vec2& value)
{
    _impl->SetUniform(name, value);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const glm::vec2* value, const uint16_t count, const uint16_t index)
{
    _impl->SetUniform(name, value, count, index);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const glm::vec3& value)
{
    _impl->SetUniform(name, value);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const glm::vec3* value, const uint16_t count, const uint16_t index)
{
    _impl->SetUniform(name, value, count, index);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const glm::vec4& value)
{
    _impl->SetUniform(name, value);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const glm::vec4* value, const uint16_t count, const uint16_t index)
{
    _impl->SetUniform(name, value, count, index);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const glm::mat4& value)
{
    _impl->SetUniform(name, value);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const glm::mat4* value, const uint16_t count, const uint16_t index)
{
    _impl->SetUniform(name, value, count, index);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const glm::mat3& value)
{
    _impl->SetUniform(name, value);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const glm::mat3* value, const uint16_t count, const uint16_t index)
{
    _impl->SetUniform(name, value, count, index);
    return *this;
}

Program& Program::SetDefine(const std::string& name, const std::string& value)
{
    _impl->SetDefine(name, value);
    return *this;
}

Program& Program::RemoveDefine(const std::string& name)
{
    _impl->RemoveDefine(name);
    return *this;
}

bool Program::GetCompiled() const
{
    return _impl->GetCompiled();
}

void Program::UseNone()
{
    Program::Impl::UseNone();
}
};
