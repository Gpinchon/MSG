#pragma once

#include <string>

namespace MSG {
struct OGLDebugGroup {
    OGLDebugGroup(const std::string& a_Name);
    ~OGLDebugGroup();
    static void Push(const std::string& a_Name);
    static void Pop();
};
}
