cmake_minimum_required(VERSION 3.25)
cmake_policy(SET CMP0140 NEW)

include(${CMAKE_CURRENT_LIST_DIR}/Generate_Program_Lib.cmake)

message(GLSL_HEADER_FILES  : ${GLSL_HEADER_FILES})
message(GLSL_STAGE_FILES   : ${GLSL_STAGE_FILES})
message(GLSL_PROGRAM_FILES : ${GLSL_PROGRAM_FILES})
message(SHADER_LIB_SRC     : ${SHADER_LIB_SRC})
message(GENERATED_DIR      : ${GENERATED_DIR})
set(CPP_CODE "")

function(MakeIncludable a_Prefix a_InputFile a_OutputFile)
  get_filename_component(FILE_NAME "${a_InputFile}" NAME)
  string(REPLACE "." "_" VAR_NAME "${a_Prefix}_${FILE_NAME}")
  file(READ ${a_InputFile} content)
  set(delim "for_c++_include")
  set(content "#pragma once\nconstexpr auto ${VAR_NAME} = R\"${delim}(\n${content}\n)${delim}\";")
  file(WRITE ${a_OutputFile} "${content}")
endfunction()

function(GenerateIncludes a_Prefix a_Files a_OutVar)
  foreach(FILE ${a_Files})
    get_filename_component(FILE_NAME "${FILE}" NAME)
    set(FILE_PATH GLSL/${a_Prefix}_${FILE_NAME})
    MakeIncludable(
        ${a_Prefix}
        ${FILE}
        ${GENERATED_DIR}/${FILE_PATH})
    string(APPEND ${a_OutVar}
    "#include <${FILE_PATH}>\n")
  endforeach()
  return(PROPAGATE ${a_OutVar})
endfunction()

function(GenerateFilesLibrary a_Prefix a_Files a_OutVar)
  string(APPEND ${a_OutVar}
  "const TabGraph::Renderer::ShaderLibrary::FilesLibrary& TabGraph::Renderer::ShaderLibrary::Get${a_Prefix}sLibrary() {\n"
  "    static const FilesLibrary lib {\n")
  foreach(file ${a_Files})
    get_filename_component(FILE_NAME ${file} NAME)
    string(REPLACE "." "_" VAR_NAME "${a_Prefix}_${FILE_NAME}")
    get_filename_component(FILE_EXT ${file} EXT)
    string(APPEND ${a_OutVar}
    "        { \"${FILE_NAME}\", ${VAR_NAME} },\n")
  endforeach()
  string(APPEND ${a_OutVar}
  "    };\n"
  "    return lib;\n"
  "}\n")
  string(APPEND ${a_OutVar}
  "\n"
  "const std::string& TabGraph::Renderer::ShaderLibrary::Get${a_Prefix}(const std::string& a_FileName) {\n"
  "    static const std::string emptyString;\n"
  "    auto& lib = Get${a_Prefix}sLibrary();\n"
  "    auto res = lib.find(a_FileName);\n"
  "    if (res != lib.end()) return res->second;\n"
  "    else {\n"
  "        std::cerr << \"Error: \" << __func__ <<\" missing file \" << a_FileName << \'\\n\';\n"
  "        return emptyString;\n"
  "    }\n"
  "}\n")
  return(PROPAGATE ${a_OutVar})
endfunction()

string(APPEND CPP_CODE
"//This file is generated at compilation time, do not edit\n"
"#include <Renderer/ShaderLibrary.hpp>\n"
"#include <Renderer/ShaderPreprocessor.hpp>\n"
"#include <iostream>\n")
string(APPEND CPP_CODE "\n")
GenerateIncludes("Header" "${GLSL_HEADER_FILES}" CPP_CODE)
GenerateIncludes("Stage" "${GLSL_STAGE_FILES}" CPP_CODE)
string(APPEND CPP_CODE "\n")
GenerateFilesLibrary("Header" "${GLSL_HEADER_FILES}" CPP_CODE)
string(APPEND CPP_CODE "\n")
GenerateFilesLibrary("Stage" "${GLSL_STAGE_FILES}" CPP_CODE)
string(APPEND CPP_CODE "\n")
GeneratePrograms("${GLSL_PROGRAM_FILES}" CPP_CODE)

file(WRITE ${SHADER_LIB_SRC} "${CPP_CODE}")
