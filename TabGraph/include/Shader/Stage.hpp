/*
* @Author: gpinchon
* @Date:   2021-01-04 09:42:57
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-13 13:10:18
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Core/Property.hpp>

#include <map>
#include <string>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Shader {
class Stage {
public:
    /**
     * code : the code to be added before main (functions for instance)
     * technique : what is to be added inside of main (function calls for instance)
    */
    struct Code {
        Code(const std::string& code = "", const std::string& technique = "")
            : code(code)
            , technique(technique) {};
        std::string code { "" };
        std::string technique { "" };
        bool operator!=(const Code& other)
        {
            return code != other.code || technique != other.technique;
        }
        Code operator+(const Code& other)
        {
            Code newCode { code, technique };
            return newCode += other;
        }
        Code operator-(const Code& other)
        {
            Code newCode { code, technique };
            return newCode -= other;
        }
        Code& operator-=(const Code& other)
        {
            size_t pos = code.find(other.code);
            if (pos != std::string::npos)
                code.erase(pos, code.length());
            pos = technique.find(other.technique);
            if (pos != std::string::npos)
                technique.erase(pos, technique.length());
            return *this;
        }
        Code& operator+=(const Code& other)
        {
            code += other.code;
            technique += other.technique;
            return *this;
        }
    };
    //ORDER IS IMPORTANT
    //Add value between last value and MaxValue
    enum class Type {
        None = -1,
        Geometry,
        Vertex,
        Fragment,
        Compute,
        TessellationEvaluation,
        TessellationControl,
        MaxValue
    };
    PROPERTY(Stage::Type, Type, Stage::Type::None);
    //PROPERTY(Stage::Code, Code, "");

public:
    Stage(const Stage& other);
    Stage(Stage::Type type = Stage::Type::None, const Code& code = { "", "" });
    Stage& operator=(const Stage& other);
    Stage& SetDefine(const std::string& name, const std::string& value = "");
    std::string GetDefine(const std::string& name) const;
    const std::map<std::string, std::string>& GetDefines() const;
    const Code& GetCode() const;
    void SetCode(const Code& code);
    Stage& operator+=(const Code& code);
    Stage& operator-=(const Code& code);

private:
    std::map<std::string, std::string> _defines {};
    Stage::Code _code { "" };
};
};
