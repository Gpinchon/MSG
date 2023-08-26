#pragma once

#include <string>

namespace TabGraph::Renderer::RAII {
struct DebugGroup {
    DebugGroup(const std::string& a_Name);
    ~DebugGroup();
};
}
