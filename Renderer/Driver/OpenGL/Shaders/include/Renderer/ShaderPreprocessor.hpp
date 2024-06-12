#pragma once

#include <string>
#include <unordered_map>

namespace TabGraph::Renderer {
class ShaderPreprocessor {
public:
    std::string ExpandCode(const std::string& a_Code);

private:
    bool _IsDefined(const std::string& a_Define);
    std::string _Value(const std::string& a_Define);
    void _Define(const std::string& a_Define, const std::string& a_Value);
    void _Undef(const std::string& a_Define);
    std::string _ExpandCode(std::stringstream& a_SS);
    std::unordered_map<std::string, std::string> _defines;
};
}