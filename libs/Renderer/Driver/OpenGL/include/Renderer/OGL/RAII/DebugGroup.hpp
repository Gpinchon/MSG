#pragma once

#include <string>

namespace MSG::Renderer::RAII {
struct DebugGroup {
    DebugGroup(const std::string& a_Name);
    ~DebugGroup();
};
}
