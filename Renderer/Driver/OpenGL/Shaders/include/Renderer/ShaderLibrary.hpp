#pragma once

#include <string>
#include <vector>

namespace TabGraph::Renderer::ShaderLibrary {

struct ProgramStage {
    std::string code;
    std::string entryPoint;
};
struct Program {
    std::vector<ProgramStage> stages;
};

std::string GetHeader(const std::string& a_Name);
std::string GetStage(const std::string& a_Name);
ProgramStage& GetProgram(const std::string& a_Name);
}
