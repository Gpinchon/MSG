#include <Renderer/OGL/RAII/Program.hpp>
#include <Renderer/OGL/RAII/Shader.hpp>

#include <GL/glew.h>

#include <stdexcept>
#include <string>

namespace TabGraph::Renderer::RAII {
static inline auto CheckProgramCompilation(GLuint a_Program)
{
    GLint result;
    glGetProgramiv(a_Program, GL_LINK_STATUS, &result);
    if (result != GL_TRUE) {
        GLsizei length { 0 };
        glGetProgramiv(a_Program, GL_INFO_LOG_LENGTH, &length);
        if (length > 1) {
            std::vector<char> infoLog(length, 0);
            glGetProgramInfoLog(a_Program, length, nullptr, infoLog.data());
            std::string logString(infoLog.begin(), infoLog.end());
            throw std::runtime_error(logString);
        } else
            throw std::runtime_error("Unknown Error");
        return false;
    }
    return true;
}

Program::Program(const std::vector<std::shared_ptr<Shader>>& a_Shaders)
    : handle(glCreateProgram())
{
    glProgramParameteri(handle, GL_PROGRAM_SEPARABLE, GL_TRUE);
    for (auto& shader : a_Shaders) {
        glAttachShader(handle, *shader);
    }
    glLinkProgram(handle);
    CheckProgramCompilation(handle);
}
Program::~Program()
{
    glDeleteProgram(handle);
}
}
