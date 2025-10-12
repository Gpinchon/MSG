#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace Msg::Renderer::ShaderLibrary {

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

using ProgramKeywords = std::vector<std::pair<std::string, std::string>>;

struct Program {
    ProgramKeywords keywords;
    std::vector<ProgramStage> stages;
};

using FilesLibrary    = std::unordered_map<std::string, std::string>;
using ProgramsLibrary = std::unordered_map<std::string, std::vector<Msg::Renderer::ShaderLibrary::Program>>;

const FilesLibrary& GetHeadersLibrary();
const FilesLibrary& GetStagesLibrary();
const std::string& GetHeader(const std::string& a_Name);
const std::string& GetStage(const std::string& a_Name);

/**
 * @brief Get the Programs Library generated durinc configuration
 *
 * @return const ProgramsLibrary&
 */
const ProgramsLibrary& GetProgramsLibrary();
/**
 * @brief Get the Program corresponding to the specified name and keywords
 *
 * @param a_Name the namme of the shader
 * @param a_Keywords the keywords to look for, if applicable
 * @return const Program&
 */
const Program& GetProgram(const std::string& a_Name, const ProgramKeywords& a_Keywords = {});
}
