#include <MSG/OGLContext.hpp>
#include <MSG/OGLShader.hpp>

#include <GL/glew.h>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace MSG {
static inline auto CheckShaderCompilation(GLuint a_Shader, const std::string& a_Code)
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
            std::cerr << a_Code << "\n"
                      << logString << std::endl;
            throw std::runtime_error(a_Code + "\n" + logString);
        } else
            throw std::runtime_error("Unknown Error");
        return false;
    }
    return true;
}

static inline auto CreateShader(OGLContext& a_Context, const unsigned a_Stage)
{
    unsigned handle = 0;
    ExecuteOGLCommand(a_Context, [&handle, &a_Stage] { handle = glCreateShader(a_Stage); }, true);
    return handle;
}

OGLShader::OGLShader(OGLContext& a_Context, const unsigned a_Stage, const std::string& a_Code)
    : handle(CreateShader(a_Context, a_Stage))
    , stage(a_Stage)
    , context(a_Context)
{
    ExecuteOGLCommand(context, [handle = handle, code = a_Code] {
        auto codePtr = code.c_str();
        glShaderSource(handle, 1, &codePtr, nullptr);
        glCompileShader(handle);
        CheckShaderCompilation(handle, code);
    });
}
OGLShader::~OGLShader()
{
    ExecuteOGLCommand(context, [handle = handle] { glDeleteShader(handle); });
}
}