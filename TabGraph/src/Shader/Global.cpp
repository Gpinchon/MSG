/*
* @Author: gpinchon
* @Date:   2021-02-22 11:10:17
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-23 19:25:55
*/
#include <Shader/Global.hpp>
#if RENDERINGAPI == OpenGL
#include <Driver/OpenGL/Shader/Global.hpp>
#endif

namespace TabGraph::Shader::Global {
void Global::SetTexture(const std::string& name, const std::shared_ptr<Textures::Texture> value)
{
    Impl::SetTexture(name, value);
}

void Global::SetUniform(const std::string& name, const float value)
{
    Impl::SetUniform(name, value);
}

void Global::SetUniform(const std::string& name, const float* value, const uint16_t count, const uint16_t index)
{
    Impl::SetUniform(name, value, count, index);
}

void Global::SetUniform(const std::string& name, const int32_t value)
{
    Impl::SetUniform(name, value);
}

void Global::SetUniform(const std::string& name, const int32_t* value, const uint16_t count, const uint16_t index)
{
    Impl::SetUniform(name, value, count, index);
}

void Global::SetUniform(const std::string& name, const uint32_t value)
{
    Impl::SetUniform(name, value);
}

void Global::SetUniform(const std::string& name, const uint32_t* value, const uint16_t count, const uint16_t index)
{
    Impl::SetUniform(name, value, count, index);
}

void SetUniform(const std::string& name, const glm::vec2& value)
{
    Impl::SetUniform(name, value);
}

void SetUniform(const std::string& name, const glm::vec2* value, const uint16_t count, const uint16_t index)
{
    Impl::SetUniform(name, value, count, index);
}

void SetUniform(const std::string& name, const glm::vec3& value)
{
    Impl::SetUniform(name, value);
}

void SetUniform(const std::string& name, const glm::vec3* value, const uint16_t count, const uint16_t index)
{
    Impl::SetUniform(name, value, count, index);
}

void SetUniform(const std::string& name, const glm::vec4& value)
{
    Impl::SetUniform(name, value);
}

void SetUniform(const std::string& name, const glm::vec4* value, const uint16_t count, const uint16_t index)
{
    Impl::SetUniform(name, value, count, index);
}

void SetUniform(const std::string& name, const glm::mat4& value)
{
    Impl::SetUniform(name, value);
}

void SetUniform(const std::string& name, const glm::mat4* value, const uint16_t count, const uint16_t index)
{
    Impl::SetUniform(name, value, count, index);
}

void SetUniform(const std::string& name, const glm::mat3& value)
{
    Impl::SetUniform(name, value);
}

void SetUniform(const std::string& name, const glm::mat3* value, const uint16_t count, const uint16_t index)
{
    Impl::SetUniform(name, value, count, index);
}

void Global::SetDefine(const std::string& name, const std::string& value)
{
    Impl::SetDefine(name, value);
}
void Global::RemoveDefine(const std::string& name)
{
    Impl::RemoveDefine(name);
}
}