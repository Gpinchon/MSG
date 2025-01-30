#include <MSG/OGLContext.hpp>
#include <MSG/OGLProgram.hpp>
#include <MSG/OGLShader.hpp>

#include <GL/glew.h>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace MSG {
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
            std::cerr << logString << std::endl;
            throw std::runtime_error(logString);
        } else
            throw std::runtime_error("Unknown Error");
        return false;
    }
    return true;
}

static inline auto CreateProgram(OGLContext& a_Context)
{
    unsigned handle = 0;
    ExecuteOGLCommand(a_Context, [&handle] { handle = glCreateProgram(); }, true);
    return handle;
}

OGLProgram::OGLProgram(OGLContext& a_Context, const std::vector<std::shared_ptr<OGLShader>>& a_Shaders)
    : handle(CreateProgram(a_Context))
    , context(a_Context)
{
    ExecuteOGLCommand(context, [handle = handle, shaders = a_Shaders] {
        glProgramParameteri(handle, GL_PROGRAM_SEPARABLE, GL_TRUE);
        for (auto& shader : shaders) {
            glAttachShader(handle, *shader);
        }
        glLinkProgram(handle);
        CheckProgramCompilation(handle);
    });
}
OGLProgram::~OGLProgram()
{
    ExecuteOGLCommand(context, [handle = handle] { glDeleteProgram(handle); });
}
}