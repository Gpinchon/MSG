#pragma once

#include <memory>
#include <string>
#include <vector>

namespace MSG {
class OGLContext;
class OGLShader;
}

namespace MSG {
class OGLProgram {
public:
    OGLProgram(OGLContext& a_Context, const std::vector<std::shared_ptr<OGLShader>>& a_Shaders);
    ~OGLProgram();
    bool GetStatus() const;
    std::string GetLog() const;
    operator unsigned() const { return handle; }
    const unsigned handle;
    OGLContext& context;
};
}
