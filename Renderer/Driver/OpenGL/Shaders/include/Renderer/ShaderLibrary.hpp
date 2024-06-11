#pragma once

#include <string>

namespace TabGraph::Renderer::ShaderLibrary {
std::string GetHeader(const std::string& a_Name);
std::string GetStage(const std::string& a_Name);
}
