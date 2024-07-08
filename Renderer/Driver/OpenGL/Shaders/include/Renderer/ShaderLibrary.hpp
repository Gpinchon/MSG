#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace TabGraph::Renderer::ShaderLibrary {

enum class StageName {
    Vertex,
    Fragment,
    Geometry,
    Tessellation,
    Compute,
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

using FilesLibrary    = std::unordered_map<std::string, std::string>;
using ProgramsLibrary = std::unordered_map<std::string, TabGraph::Renderer::ShaderLibrary::Program>;

const FilesLibrary& GetHeadersLibrary();
const FilesLibrary& GetStagesLibrary();
const std::string& GetHeader(const std::string& a_Name);
const std::string& GetStage(const std::string& a_Name);

const ProgramsLibrary& GetProgramsLibrary();
const Program& GetProgram(const std::string& a_Name);
}
