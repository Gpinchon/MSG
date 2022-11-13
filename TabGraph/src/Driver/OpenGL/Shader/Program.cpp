/*
* @Author: gpinchon
* @Date:   2021-04-13 22:39:37
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-30 02:02:37
*/
#include "Driver/OpenGL/Shader/Program.hpp"
#include "Driver/OpenGL/Shader/Global.hpp"
#include "Driver/OpenGL/Texture/Texture.hpp"
#include "Driver/OpenGL/Texture/Sampler.hpp"
#include "Driver/OpenGL/Texture/Framebuffer.hpp"
#include "Shader/Stage.hpp"

#include <GL/glew.h>
#include <array>
#include <glm/glm.hpp>
#include <string>
#include <stdexcept>

static inline bool IsTextureType(GLenum type)
{
    switch (type) {
    case GL_SAMPLER_1D:
    case GL_SAMPLER_2D:
    case GL_SAMPLER_3D:
    case GL_SAMPLER_CUBE:
    case GL_SAMPLER_1D_SHADOW:
    case GL_SAMPLER_2D_SHADOW:
    case GL_SAMPLER_1D_ARRAY:
    case GL_SAMPLER_2D_ARRAY:
    case GL_SAMPLER_1D_ARRAY_SHADOW:
    case GL_SAMPLER_2D_ARRAY_SHADOW:
    case GL_SAMPLER_2D_MULTISAMPLE:
    case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_SAMPLER_CUBE_SHADOW:
    case GL_SAMPLER_BUFFER:
    case GL_SAMPLER_2D_RECT:
    case GL_SAMPLER_2D_RECT_SHADOW:
    case GL_INT_SAMPLER_1D:
    case GL_INT_SAMPLER_2D:
    case GL_INT_SAMPLER_3D:
    case GL_INT_SAMPLER_CUBE:
    case GL_INT_SAMPLER_1D_ARRAY:
    case GL_INT_SAMPLER_2D_ARRAY:
    case GL_INT_SAMPLER_2D_MULTISAMPLE:
    case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_INT_SAMPLER_BUFFER:
    case GL_INT_SAMPLER_2D_RECT:
    case GL_UNSIGNED_INT_SAMPLER_1D:
    case GL_UNSIGNED_INT_SAMPLER_2D:
    case GL_UNSIGNED_INT_SAMPLER_3D:
    case GL_UNSIGNED_INT_SAMPLER_CUBE:
    case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_BUFFER:
    case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
    case GL_IMAGE_1D:
    case GL_IMAGE_2D:
    case GL_IMAGE_3D:
    case GL_IMAGE_2D_RECT:
    case GL_IMAGE_CUBE:
    case GL_IMAGE_BUFFER:
    case GL_IMAGE_1D_ARRAY:
    case GL_IMAGE_2D_ARRAY:
    case GL_IMAGE_2D_MULTISAMPLE:
    case GL_IMAGE_2D_MULTISAMPLE_ARRAY:
    case GL_INT_IMAGE_1D:
    case GL_INT_IMAGE_2D:
    case GL_INT_IMAGE_3D:
    case GL_INT_IMAGE_2D_RECT:
    case GL_INT_IMAGE_CUBE:
    case GL_INT_IMAGE_BUFFER:
    case GL_INT_IMAGE_1D_ARRAY:
    case GL_INT_IMAGE_2D_ARRAY:
    case GL_INT_IMAGE_2D_MULTISAMPLE:
    case GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
    case GL_UNSIGNED_INT_IMAGE_1D:
    case GL_UNSIGNED_INT_IMAGE_2D:
    case GL_UNSIGNED_INT_IMAGE_3D:
    case GL_UNSIGNED_INT_IMAGE_2D_RECT:
    case GL_UNSIGNED_INT_IMAGE_CUBE:
    case GL_UNSIGNED_INT_IMAGE_BUFFER:
    case GL_UNSIGNED_INT_IMAGE_1D_ARRAY:
    case GL_UNSIGNED_INT_IMAGE_2D_ARRAY:
    case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE:
    case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
        return true;
    default:
        return false;
    }
}

std::array<GLenum, 6> GLStageLUT = {
    GL_GEOMETRY_SHADER, //Geometry
    GL_VERTEX_SHADER, //Vertex
    GL_FRAGMENT_SHADER, //Fragment
    GL_COMPUTE_SHADER, //Compute
    GL_TESS_EVALUATION_SHADER, //TessellationEvaluation
    GL_TESS_CONTROL_SHADER //TessellationControl
};

namespace TabGraph::Shader {
Program::Impl::~Impl()
{
    glDeleteProgram(GetHandle());
}
const Program::Impl::Handle& Program::Impl::GetHandle() const
{
    return _handle;
}
void Program::Impl::Attach(const Stage& stage)
{
    _stages.at((int)stage.GetType()) = stage;
}
Stage& Program::Impl::GetStage(Stage::Type type)
{
    return _stages.at((int)type);
}
void Program::Impl::Compile()
{
    if (GetCompiled())
        return;
    _SetHandle(glCreateProgram());
    std::array<GLuint, (int)Stage::Type::MaxValue> handles { 0 };
    for (const auto& stage : _stages) {
        if (stage.GetCode().code.empty() && stage.GetCode().technique.empty())
            continue;
        std::string fullCode = "#version " + GetGLSLVersion() + '\n';
        for (const auto& define : _defines)
            fullCode += "#define " + define.first + ' ' + define.second + '\n';
        for (const auto& define : stage.GetDefines())
            fullCode += "#define " + define.first + ' ' + define.second + '\n';
        if (!stage.GetCode().code.empty())
            fullCode += stage.GetCode().code + '\n';
        fullCode += "void main(void) {\n";
        if (!stage.GetCode().technique.empty())
            fullCode += stage.GetCode().technique + '\n';
        fullCode += "}\n";
        auto codeBuff { fullCode.c_str() };
        auto handle = handles.at((int)stage.GetType()) = glCreateShader(GLStageLUT.at((int)stage.GetType()));
        glShaderSource(handle, 1, &codeBuff, nullptr);
        glCompileShader(handle);
        GLint result;
        GLint loglength;
        result = GL_FALSE;
        glGetShaderiv(handle, GL_COMPILE_STATUS, &result);
        if (result != GL_TRUE) {
            glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &loglength);
            if (loglength > 1) {
                std::vector<char> log(loglength, 0);
                glGetShaderInfoLog(handle, loglength, nullptr, log.data());
                std::string logString(log.begin(), log.end());
                throw std::runtime_error(logString);
            } else {
                throw std::runtime_error("Unknown Error");
            }
        }
        glAttachShader(GetHandle(), handle);
    }
    glLinkProgram(GetHandle());
    GLint result;
    GLint loglength;

    result = GL_FALSE;
    glGetProgramiv(GetHandle(), GL_LINK_STATUS, &result);
    if (result != GL_TRUE) {
        glGetProgramiv(GetHandle(), GL_INFO_LOG_LENGTH, &loglength);
        if (loglength > 1) {
            std::vector<char> log(loglength, 0);
            glGetProgramInfoLog(GetHandle(), loglength, nullptr, log.data());
            std::string logString(log.begin(), log.end());
            throw std::runtime_error(logString);
        } else {
            throw std::runtime_error("Unknown Error");
        }
    }
    for (const auto& handle : handles) {
        if (handle == 0)
            continue;
        glDetachShader(GetHandle(), handle);
        glDeleteShader(handle);
    }
    GLint numUniforms = 0;
    glGetProgramiv(GetHandle(), GL_ACTIVE_UNIFORMS, &numUniforms);
    uint16_t textureIndex { 0 };
    char name[4096];
    while (--numUniforms >= 0) {
        GLenum type;
        GLint size;
        glGetActiveUniform(GetHandle(), static_cast<GLuint>(numUniforms), 4096, nullptr, &size, &type, name);
        auto loc { glGetUniformLocation(GetHandle(), name) };
        _uniformLocs[name] = loc;
        if (IsTextureType(type))
            _textureIndice[loc] = ++textureIndex;
    }
    _SetCompiled(true);
}

void Program::Impl::SetTexture(const std::string& name, const std::shared_ptr<Textures::Texture> value)
{
    auto loc { _uniformLocs.find(name) };
    if (loc != _uniformLocs.end()) {
        auto index { _textureIndice.find(loc->second) };
        if (index != _textureIndice.end()) {
            glActiveTexture(GL_TEXTURE0 + index->second);
            if (value != nullptr) {
                value->Load();
                glBindTexture(OpenGL::GetEnum(value->GetType()), value->GetImpl().GetHandle());
                glBindSampler(index->second, value->GetSampler()->GetImpl().GetHandle());
            } else {
                glBindTexture(OpenGL::GetEnum(Textures::Texture::Type::Texture1D), 0);
                glBindTexture(OpenGL::GetEnum(Textures::Texture::Type::Texture1DArray), 0);
                glBindTexture(OpenGL::GetEnum(Textures::Texture::Type::Texture2D), 0);
                glBindTexture(OpenGL::GetEnum(Textures::Texture::Type::Texture2DArray), 0);
                glBindTexture(OpenGL::GetEnum(Textures::Texture::Type::Texture2DMultisample), 0);
                glBindTexture(OpenGL::GetEnum(Textures::Texture::Type::Texture2DMultisampleArray), 0);
                glBindTexture(OpenGL::GetEnum(Textures::Texture::Type::Texture3D), 0);
                glBindTexture(OpenGL::GetEnum(Textures::Texture::Type::TextureCubemap), 0);
                glBindTexture(OpenGL::GetEnum(Textures::Texture::Type::TextureRectangle), 0);
                glBindTexture(OpenGL::GetEnum(Textures::Texture::Type::TextureCubemapArray), 0);
                glBindSampler(index->second, 0);
            }
            glUniform1i(loc->second, index->second);
        }
    }
}
void Program::Impl::SetUniform(const std::string& name, const float value)
{
    SetUniform(name, &value, 1, 0);
}
void Program::Impl::SetUniform(const std::string& name, const float* value, const uint16_t count, const uint16_t index)
{
    auto loc { _uniformLocs.find(name) };
    if (loc != _uniformLocs.end())
        glUniform1fv(loc->second + index, count, value);
}
void Program::Impl::SetUniform(const std::string& name, const int32_t value)
{
    SetUniform(name, &value, 1, 0);
}
void Program::Impl::SetUniform(const std::string& name, const int32_t* value, const uint16_t count, const uint16_t index)
{
    auto loc { _uniformLocs.find(name) };
    if (loc != _uniformLocs.end())
        glUniform1iv(loc->second + index, count, value);
}
void Program::Impl::SetUniform(const std::string& name, const uint32_t value)
{
    SetUniform(name, &value, 1, 0);
}
void Program::Impl::SetUniform(const std::string& name, const uint32_t* value, const uint16_t count, const uint16_t index)
{
    auto loc { _uniformLocs.find(name) };
    if (loc != _uniformLocs.end())
        glUniform1uiv(loc->second + index, count, value);
}
void Program::Impl::SetUniform(const std::string& name, const glm::vec2& value)
{
    SetUniform(name, &value, 1, 0);
}
void Program::Impl::SetUniform(const std::string& name, const glm::vec2* value, const uint16_t count, const uint16_t index)
{
    auto loc { _uniformLocs.find(name) };
    if (loc != _uniformLocs.end())
        glUniform2fv(loc->second + index, count, (float*)value);
}
void Program::Impl::SetUniform(const std::string& name, const glm::vec3& value)
{
    SetUniform(name, &value, 1, 0);
}
void Program::Impl::SetUniform(const std::string& name, const glm::vec3* value, const uint16_t count, const uint16_t index)
{
    auto loc { _uniformLocs.find(name) };
    if (loc != _uniformLocs.end())
        glUniform3fv(loc->second + index, count, (float*)value);
}
void Program::Impl::SetUniform(const std::string& name, const glm::vec4& value)
{
    SetUniform(name, &value, 1, 0);
}
void Program::Impl::SetUniform(const std::string& name, const glm::vec4* value, const uint16_t count, const uint16_t index)
{
    auto loc { _uniformLocs.find(name) };
    if (loc != _uniformLocs.end())
        glUniform4fv(loc->second + index, count, (float*)value);
}
void Program::Impl::SetUniform(const std::string& name, const glm::mat4& value)
{
    SetUniform(name, &value, 1, 0);
}

void Program::Impl::SetUniform(const std::string& name, const glm::mat4* value, const uint16_t count, const uint16_t index)
{
    auto loc { _uniformLocs.find(name) };
    if (loc != _uniformLocs.end())
        glUniformMatrix4fv(loc->second + index, count, false, (float*)value);
}

void Program::Impl::SetUniform(const std::string& name, const glm::mat3& value)
{
    SetUniform(name, &value, 1, 0);
}

void Program::Impl::SetUniform(const std::string& name, const glm::mat3* value, const uint16_t count, const uint16_t index)
{
    auto loc { _uniformLocs.find(name) };
    if (loc != _uniformLocs.end())
        glUniformMatrix3fv(loc->second + index, count, false, (float*)value);
}

void Program::Impl::SetDefine(const std::string& name, const std::string& value)
{
    auto& define { _defines.find(name) };
    if (define == _defines.end() || define->second != value) {
        _defines[name] = value;
        glDeleteProgram(GetHandle());
        _SetHandle(0);
        _SetCompiled(false);
    }
}

void Program::Impl::RemoveDefine(const std::string& name)
{
    if (_defines.erase(name) > 0) {
        glDeleteProgram(GetHandle());
        _SetHandle(0);
        _SetCompiled(false);
    }
}

void Program::Impl::Use()
{
    for (const auto& globalDefine : Global::Impl::GetDefines())
        SetDefine(globalDefine.first, globalDefine.second);
    if (!GetCompiled())
        Compile();
    glUseProgram(GetHandle());
    for (const auto& globalVec2 : Global::Impl::GetVec2())
        SetUniform(globalVec2.first, globalVec2.second.data(), globalVec2.second.size(), 0);
    for (const auto& globalVec3 : Global::Impl::GetVec3())
        SetUniform(globalVec3.first, globalVec3.second.data(), globalVec3.second.size(), 0);
    for (const auto& globalVec4 : Global::Impl::GetVec4())
        SetUniform(globalVec4.first, globalVec4.second.data(), globalVec4.second.size(), 0);
    for (const auto& globalMat4 : Global::Impl::GetMat4())
        SetUniform(globalMat4.first, globalMat4.second.data(), globalMat4.second.size(), 0);
    for (const auto& globalMat3 : Global::Impl::GetMat3())
        SetUniform(globalMat3.first, globalMat3.second.data(), globalMat3.second.size(), 0);
    for (const auto& globalFloat : Global::Impl::GetFloats())
        SetUniform(globalFloat.first, globalFloat.second.data(), globalFloat.second.size(), 0);
    for (const auto& globalInt : Global::Impl::GetInts())
        SetUniform(globalInt.first, globalInt.second.data(), globalInt.second.size(), 0);
    for (const auto& globalUint : Global::Impl::GetUints())
        SetUniform(globalUint.first, globalUint.second.data(), globalUint.second.size(), 0);
    for (const auto& globalTexture : Global::Impl::GetTextures())
        SetTexture(globalTexture.first, globalTexture.second);
}

void Program::Impl::UseNone()
{
    glUseProgram(0);
}

void Program::Impl::_SetHandle(uint32_t value)
{
    _handle = value;
}
};
