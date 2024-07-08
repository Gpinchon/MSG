cmake_minimum_required(VERSION 3.25)
cmake_policy(SET CMP0140 NEW)

include(${CMAKE_CURRENT_LIST_DIR}/Generate_Program_Lib.cmake)

message(GLSL_HEADER_FILES  : ${GLSL_HEADER_FILES})
message(GLSL_STAGE_FILES   : ${GLSL_STAGE_FILES})
message(GLSL_PROGRAM_FILES : ${GLSL_PROGRAM_FILES})
message(SHADER_LIB_SRC     : ${SHADER_LIB_SRC})
message(GENERATED_DIR      : ${GENERATED_DIR})
set(CPP_CODE "")

function(MakeIncludable a_InputFile a_OutputFile a_Prefix)
  get_filename_component(FILE_NAME ${a_InputFile} NAME)
  string(REPLACE "." "_" FILE_NAME ${FILE_NAME})
  file(READ ${a_InputFile} content)
  set(delim "for_c++_include")
  set(content "#pragma once\nconstexpr auto ${a_Prefix}${FILE_NAME} = R\"${delim}(\n${content}\n)${delim}\";")
  file(WRITE ${a_OutputFile} "${content}")
endfunction()

function(GenerateIncludes a_Files a_TargetDir a_Prefix a_OutVar)
  foreach(file ${a_Files})
    get_filename_component(FILE_NAME ${file} NAME)
    set(FILE_PATH ${a_TargetDir}/${FILE_NAME})
    MakeIncludable(
        ${file}
        ${GENERATED_DIR}/${FILE_PATH}
        ${a_Prefix})
    string(APPEND ${a_OutVar}
    "#include <${FILE_PATH}>\n")
  endforeach()
  return(PROPAGATE ${a_OutVar})
endfunction()

function(GenerateFunction a_FunctionName a_Files a_Prefix a_OutVar)
  string(APPEND ${a_OutVar}
  "std::string TabGraph::Renderer::ShaderLibrary::${a_FunctionName}(const std::string& a_FileName) {\n"
  "    static const Library lib {\n")
  foreach(file ${a_Files})
    get_filename_component(FILE_NAME ${file} NAME)
    string(REPLACE "." "_" VAR_NAME ${FILE_NAME})
    get_filename_component(FILE_EXT ${file} EXT)
    string(APPEND ${a_OutVar}
    "        { \"${FILE_NAME}\", ${a_Prefix}${VAR_NAME} },\n")
  endforeach()
  string(APPEND ${a_OutVar}
  "    };\n"
  "    auto res = lib.find(a_FileName);\n"
  "    if (res != lib.end()) return res->second;\n"
  "    else {\n"
  "        std::cerr << \"Error: \" << __func__ <<\" missing file \" << a_FileName << \'\\n\';\n"
  "        return \"\";\n"
  "    }\n"
  "}\n")
  return(PROPAGATE ${a_OutVar})
endfunction()

string(APPEND CPP_CODE
"//This generates the default shader libary\n"
"#include <string>\n"
"#include <unordered_map>\n"
"#include <iostream>\n"
"#include <Renderer/ShaderLibrary.hpp>\n"
"#include <Renderer/ShaderPreprocessor.hpp>\n")
string(APPEND CPP_CODE "\n")
GenerateIncludes("${GLSL_HEADER_FILES}" "GLSL/header" "HEADER_" CPP_CODE)
GenerateIncludes("${GLSL_STAGE_FILES}" "GLSL/stage" "STAGE_" CPP_CODE)
string(APPEND CPP_CODE "\n")
string(APPEND CPP_CODE
"using Library = std::unordered_map<std::string, std::string>;\n")
string(APPEND CPP_CODE "\n")
GenerateFunction("GetHeader" "${GLSL_HEADER_FILES}" "HEADER_" CPP_CODE)
string(APPEND CPP_CODE "\n")
GenerateFunction("GetStage" "${GLSL_STAGE_FILES}" "STAGE_" CPP_CODE)
string(APPEND CPP_CODE "\n")
GeneratePrograms("${GLSL_PROGRAM_FILES}" CPP_CODE)

file(WRITE ${SHADER_LIB_SRC} "${CPP_CODE}")
