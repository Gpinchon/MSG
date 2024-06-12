#include <Renderer/ShaderLibrary.hpp>
#include <Renderer/ShaderPreprocessor.hpp>

#include <sstream>
#include <vector>

namespace TabGraph::Renderer {
bool ShaderPreprocessor::_IsDefined(const std::string& a_Define)
{
    auto res = _defines.find(a_Define);
    return res != _defines.end();
}

std::string ShaderPreprocessor::_Value(const std::string& a_Define)
{
    return _defines.at(a_Define);
}

void ShaderPreprocessor::_Define(const std::string& a_Define, const std::string& a_Value)
{
    _defines[a_Define] = a_Value;
}

void ShaderPreprocessor::_Undef(const std::string& a_Define)
{
    _defines.erase(a_Define);
}

struct IfStack {
    bool isIfDef;
    bool keep;
};

std::string ShaderPreprocessor::_ExpandCode(std::stringstream& a_SS)
{
    std::string line;
    std::string expandedCode;
    std::vector<IfStack> ifStack(1, { true, true });
    while (std::getline(a_SS, line)) {
        if (!line.empty() && line.at(0) == '#') {
            std::stringstream lineStream(line);
            std::string key;
            lineStream >> key;
            if (key == "#include") {
                std::string path;
                std::string incl;
                lineStream >> path;
                path = { path.begin() + 1, path.end() - 1 };
                expandedCode += ExpandCode(ShaderLibrary::GetHeader(path));
                continue;
            } else if (key == "#define") {
                std::string define;
                std::string value;
                lineStream >> define;
                std::getline(lineStream, value);
                _Define(define, value);
            } else if (key == "#undef") {
                std::string define;
                lineStream >> define;
                _Undef(define);
            } else if (key == "#ifdef") {
                std::string value;
                lineStream >> value;
                IfStack newIf {
                    .isIfDef = true,
                    .keep    = _IsDefined(value)
                };
                ifStack.push_back(newIf);
                continue;
            } else if (key == "#ifndef") {
                std::string value;
                lineStream >> value;
                IfStack newIf {
                    .isIfDef = true,
                    .keep    = !_IsDefined(value)
                };
                ifStack.push_back(newIf);
                continue;
            } else if (key == "#else" && ifStack.back().isIfDef) {
                ifStack.back().keep = !ifStack.back().keep;
                continue;
            } else if (key == "#if") {
                auto newIf    = ifStack.back();
                newIf.isIfDef = false;
                ifStack.push_back(newIf);
            } else if (key == "#endif") {
                auto isIfDef = ifStack.back().isIfDef;
                ifStack.pop_back();
                if (isIfDef)
                    continue;
            }
        }
        if (!ifStack.back().keep)
            continue;
        expandedCode += line + '\n';
    }
    return expandedCode;
}

std::string ShaderPreprocessor::ExpandCode(const std::string& a_Code)
{
    auto sStream      = std::stringstream(a_Code);
    auto expandedCode = _ExpandCode(sStream);
    return expandedCode;
}
}
