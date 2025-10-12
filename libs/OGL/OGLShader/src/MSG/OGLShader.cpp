#include <MSG/OGLContext.hpp>
#include <MSG/OGLShader.hpp>

#include <GL/glew.h>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace Msg {
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
    });
}
OGLShader::~OGLShader()
{
    ExecuteOGLCommand(context, [handle = handle] { glDeleteShader(handle); });
}

bool OGLShader::GetStatus() const
{
    GLint status;
    ExecuteOGLCommand(context, [handle = handle, &status] { glGetShaderiv(handle, GL_COMPILE_STATUS, &status); }, true);
    return status == GL_TRUE;
}

std::string OGLShader::GetLog() const
{
    std::string log;
    ExecuteOGLCommand(context, [handle = handle, &log] {
        GLsizei length { 0 };
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &length);
        if (length > 1) {
            std::vector<char> infoLog(length, 0);
            glGetShaderInfoLog(handle, length, nullptr, infoLog.data());
            log = {infoLog.begin(), infoLog.end()};
        } }, true);
    return log;
}
}
