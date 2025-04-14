#include <MSG/OGLContext.hpp>
#include <MSG/OGLProgram.hpp>
#include <MSG/OGLShader.hpp>

#include <GL/glew.h>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace MSG {
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
    });
}
OGLProgram::~OGLProgram()
{
    ExecuteOGLCommand(context, [handle = handle] { glDeleteProgram(handle); });
}

bool OGLProgram::GetStatus() const
{
    GLint status;
    ExecuteOGLCommand(context, [handle = handle, &status] { glGetProgramiv(handle, GL_LINK_STATUS, &status); }, true);
    return status == GL_TRUE;
}

std::string OGLProgram::GetLog() const
{
    std::string log;
    ExecuteOGLCommand(context, [handle = handle, &log] {
        GLsizei length { 0 };
        glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length);
        if (length > 1) {
            std::vector<char> infoLog(length, 0);
            glGetProgramInfoLog(handle, length, nullptr, infoLog.data());
            log = {infoLog.begin(), infoLog.end()};
        } }, true);
    return log;
}
}