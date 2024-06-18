#pragma once

#include <string>
#include <vector>

namespace TabGraph::Renderer::ShaderLibrary {

enum class StageName {
    Vertex,
    Fragment,
    Geometry,
    Tessellation,
    MaxValue
};

struct ProgramStage {
    StageName name;
    std::string entryPoint;
    std::string code;
};

struct Program {
    std::vector<ProgramStage> stages;
};

std::string GetHeader(const std::string& a_Name);
std::string GetStage(const std::string& a_Name);
const Program& GetProgram(const std::string& a_Name);
}
