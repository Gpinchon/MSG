#include <Renderer/ShaderLibrary.hpp>
#include <Renderer/ShaderPreprocessor.hpp>

#include <sstream>

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

std::string ShaderPreprocessor::_ParseLine(std::stringstream& a_SS, const std::string& a_Line)
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
            line = _ParseIfDef(a_SS, _IsDefined(value));
        } else if (key == "#ifndef") {
            std::string value;
            lineStream >> value;
            line = _ParseIfDef(a_SS, !_IsDefined(value));
        } else if (key == "#if") {
            line += _ParseIf(a_SS);
        } else if (key == "#define") {
            std::string define, value;
            lineStream >> define;
            std::getline(lineStream, value);
            _Define(define, value);
        } else if (key == "#undef") {
            std::string define;
            lineStream >> define;
            _Undef(define);
        } else if (key == "#include") {
            std::string path, incl;
            lineStream >> path;
            path = { path.begin() + 1, path.end() - 1 };
            line = ExpandCode(ShaderLibrary::GetHeader(path));
        }
    }
    return line + "\n";
}

std::string ShaderPreprocessor::_ParseIfDef(std::stringstream& a_SS, bool a_Cond)
{
    std::string line;
    std::string code;
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
            auto ifCode = _ParseIf(a_SS);
            if (a_Cond)
                code += line + "\n" + ifCode;
        } else if (a_Cond) {
            code += _ParseLine(a_SS, line);
        }
    }
    return code;
}

std::string ShaderPreprocessor::_ParseIf(std::stringstream& a_SS)
{
    std::string line;
    std::string code;
    while (std::getline(a_SS, line)) {
        if (line.empty())
            continue;
        code += _ParseLine(a_SS, line);
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

std::string ShaderPreprocessor::_ExpandCode(std::stringstream& a_SS)
{
    std::string line;
    std::string expandedCode;
    while (std::getline(a_SS, line)) {
        expandedCode += _ParseLine(a_SS, line);
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
