#include <Renderer/OGL/RAII/Shader.hpp>
#include <Renderer/OGL/ShaderCompiler.hpp>
#include <Renderer/OGL/ShaderLibrary.hpp>

#include <Tools/LazyConstructor.hpp>

#include <regex>
#include <sstream>

namespace TabGraph::Renderer {
struct Preprocessor {
    bool IsDefined(const std::string& a_Define)
    {
        auto res = defines.find(a_Define);
        return res != defines.end();
    }
    std::string Value(const std::string& a_Define)
    {
        return defines.at(a_Define);
    }
    void Define(const std::string& a_Define, const std::string& a_Value)
    {
        defines[a_Define] = a_Value;
    }
    void Undef(const std::string& a_Define)
    {
        defines.erase(a_Define);
    }
    std::string ParseLine(std::stringstream& a_SS, const std::string& a_Line)
    {
        auto last = a_Line.find_last_not_of(' ');
        if (last == std::string::npos)
            return "";
        auto line = a_Line.substr(0, last + 1);
        if (!line.empty() && line.at(0) == '#') {
            std::stringstream lineStream(line);
            std::string key;
            lineStream >> key;
            if (key == "#ifdef") {
                std::string value;
                lineStream >> value;
                line = ParseIfDef(a_SS, IsDefined(value));
            } else if (key == "#ifndef") {
                std::string value;
                lineStream >> value;
                line = ParseIfDef(a_SS, !IsDefined(value));
            } else if (key == "#if") {
                line += ParseIf(a_SS);
            } else if (key == "#define") {
                std::string define, value;
                lineStream >> define;
                std::getline(lineStream, value);
                Define(define, value);
            } else if (key == "#undef") {
                std::string define;
                lineStream >> define;
                Undef(define);
            } else if (key == "#include") {
                std::string path, incl;
                lineStream >> path;
                path = { path.begin() + 1, path.end() - 1 };
                line = ExpandCode(ShaderLibrary::GetCode(path));
            }
        }
        return line + "\n";
    }
    std::string ParseIfDef(std::stringstream& a_SS, bool a_Cond)
    {
        std::string line, code;
        while (std::getline(a_SS, line)) {
            if (line.empty())
                continue;
            std::stringstream lineStream(line);
            std::string key;
            lineStream >> key;
            if (key == "#endif") {
                break;
            } else if (key == "#else") {
                a_Cond = !a_Cond;
            } else if (key == "#if") {
                auto ifCode = ParseIf(a_SS);
                if (a_Cond)
                    code += line + "\n" + ifCode;
            } else if (a_Cond) {
                code += ParseLine(a_SS, line);
            }
        }
        return code;
    }
    std::string ParseIf(std::stringstream& a_SS)
    {
        std::string line, code;
        while (std::getline(a_SS, line)) {
            if (line.empty())
                continue;
            code += ParseLine(a_SS, line);
            if (line.at(0) == '#') {
                std::stringstream lineStream(line);
                std::string key;
                lineStream >> key;
                if (key == "#endif")
                    break;
            }
        }
        return code;
    }
    std::string ExpandCode(std::stringstream& a_SS)
    {
        std::string line, expandedCode;
        while (std::getline(a_SS, line)) {
            expandedCode += ParseLine(a_SS, line);
        }
        return expandedCode;
    }
    std::string ExpandCode(const std::string& a_Code)
    {
        auto expandedCode = ExpandCode(std::stringstream(a_Code));
        return expandedCode;
    }
    std::unordered_map<std::string, std::string> defines;
};

ShaderCompiler::ShaderCompiler(RAII::Context& a_Context)
    : context(a_Context)
{
}
RAII::Shader& ShaderCompiler::CompileShader(
    unsigned a_Stage,
    const std::string& a_Code)
{
    auto lazyConstructor = Tools::LazyConstructor([this, a_Stage, a_Code] {
        return RAII::MakePtr<RAII::Shader>(context, a_Stage, Preprocessor {}.ExpandCode(a_Code).c_str());
    });
    return *GetOrCreate(a_Stage, a_Code, lazyConstructor);
}
}
