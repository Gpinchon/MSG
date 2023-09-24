#include <Renderer/OGL/RAII/Shader.hpp>

#include <GL/glew.h>
#include <stdexcept>
#include <string>
#include <vector>

namespace TabGraph::Renderer::RAII {
static inline auto CheckShaderCompilation(GLuint a_Shader)
{
    GLint result;
    glGetShaderiv(a_Shader, GL_COMPILE_STATUS, &result);
    if (result != GL_TRUE) {
        GLsizei length { 0 };
        glGetShaderiv(a_Shader, GL_INFO_LOG_LENGTH, &length);
        if (length > 1) {
            std::vector<char> infoLog(length, 0);
            glGetShaderInfoLog(a_Shader, length, nullptr, infoLog.data());
            std::string logString(infoLog.begin(), infoLog.end());
            throw std::runtime_error(logString);
        } else
            throw std::runtime_error("Unknown Error");
        return false;
    }
    return true;
}

Shader::Shader(const unsigned a_Stage, const char* a_Code)
    : handle(glCreateShader(a_Stage))
    , stage(a_Stage)
{
    glShaderSource(
        handle,
        1, &a_Code, nullptr);
    glCompileShader(handle);
    CheckShaderCompilation(handle);
}
Shader::~Shader()
{
    glDeleteShader(handle);
}
}
