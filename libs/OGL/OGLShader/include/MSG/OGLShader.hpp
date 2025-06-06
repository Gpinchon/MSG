#pragma once

#include <string>

namespace MSG {
class OGLContext;
}

namespace MSG {
struct OGLShader {
    OGLShader(
        OGLContext& a_Context,
        const unsigned a_Stage,
        const std::string& a_Code);
    ~OGLShader();
    bool GetStatus() const;
    std::string GetLog() const;
    operator unsigned() const { return handle; }
    const unsigned handle;
    const unsigned stage;
    OGLContext& context;
};
}
