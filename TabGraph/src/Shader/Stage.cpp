/*
* @Author: gpinchon
* @Date:   2020-10-01 15:30:48
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-13 22:35:42
*/

#include <Shader/Stage.hpp>

namespace TabGraph::Shader {
Stage::Stage(Stage::Type type, const Code& code)
{
    SetType(type);
    SetCode(code);
}

Stage::Stage(const Stage& other)
    : Stage(other.GetType(), other.GetCode())
{
    _defines = other._defines;
}

//WHY ?! WHY DO YOU NEED THIS ?!
Stage& Stage::operator=(const Stage& other)
{
    SetType(other.GetType());
    SetCode(other.GetCode());
    _defines = other._defines;
    return *this;
}

Stage& Shader::Stage::SetDefine(const std::string& name, const std::string& value)
{
    _defines[name] = value;
    return *this;
}

std::string Stage::GetDefine(const std::string& name) const
{
    auto value { _defines.find(name) };
    if (value != _defines.end())
        return value->second;
    return "";
}

const std::map<std::string, std::string>& Stage::GetDefines() const
{
    return _defines;
}

const Stage::Code& Stage::GetCode() const
{
    return _code;
}

void Stage::SetCode(const Code& code)
{
    _code = code;
}

Stage& Stage::operator+=(const Code& code)
{
    _code += code;
    return *this;
}
Stage& Stage::operator-=(const Code& code)
{
    _code -= code;
    return *this;
}
}
