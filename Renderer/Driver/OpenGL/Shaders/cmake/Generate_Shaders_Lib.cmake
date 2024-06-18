cmake_minimum_required(VERSION 3.25)
cmake_policy(SET CMP0140 NEW)

message(GLSL_HEADER_FILES  : ${GLSL_HEADER_FILES})
message(GLSL_STAGE_FILES   : ${GLSL_STAGE_FILES})
message(GLSL_PROGRAM_FILES : ${GLSL_PROGRAM_FILES})
message(SHADER_LIB_SRC     : ${SHADER_LIB_SRC})
message(GENERATED_DIR      : ${GENERATED_DIR})

function(MakeIncludable a_InputFile a_OutputFile)
  file(READ ${a_InputFile} content)
  set(delim "for_c++_include")
  set(content "R\"${delim}(\n${content})${delim}\"")
  file(WRITE ${a_OutputFile} "${content}")
endfunction()

function(GenerateIncludes files target_dir prefix)
  foreach(file ${files})
    get_filename_component(FILE_NAME ${file} NAME_WE)
    get_filename_component(FILE_EXT ${file} EXT)
    set(FILE_PATH ${target_dir}/${FILE_NAME}${FILE_EXT})
    MakeIncludable(
        ${file}
        ${GENERATED_DIR}/${FILE_PATH})
    file(APPEND ${SHADER_LIB_SRC}
    "constexpr auto ${prefix}${FILE_NAME} =\n"
    "    #include <${FILE_PATH}>\n"
    ";\n")
  endforeach()
endfunction()

function(GenerateFunction a_FunctionName a_Files a_Prefix)
  file(APPEND ${SHADER_LIB_SRC} 
  "\n"
  "std::string TabGraph::Renderer::ShaderLibrary::${a_FunctionName}(const std::string& a_FileName) {\n"
  "    static const Library lib {\n")

  foreach(file ${a_Files})
    get_filename_component(FILE_NAME ${file} NAME_WE)
    get_filename_component(FILE_EXT ${file} EXT)
    file(APPEND ${SHADER_LIB_SRC}
    "        { \"${FILE_NAME}${FILE_EXT}\", ${a_Prefix}${FILE_NAME} },\n")
  endforeach()

  file(APPEND ${SHADER_LIB_SRC}
  "    };\n"
  "    auto res = lib.find(a_FileName);\n"
  "    return res != lib.end() ? res->second : \"\";\n"
  "}\n")
endfunction()

file(WRITE ${SHADER_LIB_SRC}
"//This generates the default shader libary\n"
"#include <string>\n"
"#include <unordered_map>\n"
"#include <Renderer/ShaderLibrary.hpp>\n"
"#include <Renderer/ShaderPreprocessor.hpp>\n\n")
file(APPEND ${SHADER_LIB_SRC} 
  "\nusing Library = std::unordered_map<std::string, std::string>;\n")
GenerateIncludes("${GLSL_HEADER_FILES}" "headers" "HEADER_")
GenerateIncludes("${GLSL_STAGE_FILES}" "stage" "STAGE_")
GenerateFunction("GetHeader" "${GLSL_HEADER_FILES}" "HEADER_")
GenerateFunction("GetStage" "${GLSL_STAGE_FILES}" "STAGE_")

include(${CMAKE_CURRENT_LIST_DIR}/Generate_Program_Lib.cmake)
