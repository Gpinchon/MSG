cmake_minimum_required(VERSION 3.25)
cmake_policy(SET CMP0140 NEW)

message(GLSL_HEADER_FILES : ${GLSL_HEADER_FILES})
message(GLSL_STAGE_FILES : ${GLSL_STAGE_FILES})
message(SHADER_LIB_SRC: ${SHADER_LIB_SRC})
message(GENERATED_DIR: ${GENERATED_DIR})

function(make_includable input_file output_file)
  file(READ ${input_file} content)
  set(delim "for_c++_include")
  set(content "R\"${delim}(\n${content})${delim}\"")
  file(WRITE ${output_file} "${content}")
endfunction(make_includable)

function(GenerateIncludes files target_dir prefix)
  foreach(file ${files})
    get_filename_component(FILE_NAME ${file} NAME_WE)
    get_filename_component(FILE_EXT ${file} EXT)
    set(FILE_PATH ${target_dir}/${FILE_NAME}${FILE_EXT})
    make_includable(
        ${file}
        ${GENERATED_DIR}/${FILE_PATH})
    file(APPEND ${SHADER_LIB_SRC}
    "constexpr auto ${prefix}${FILE_NAME} =\n"
    "    #include <${FILE_PATH}>\n"
    ";\n")
  endforeach()
endfunction(GenerateIncludes)

function(GenerateFunction function_name files prefix)
  file(APPEND ${SHADER_LIB_SRC} 
  "\n"
  "std::string TabGraph::Renderer::ShaderLibrary::${function_name}(const std::string& a_FileName) {\n"
  "    static const Library lib {\n")

  foreach(file ${files})
    get_filename_component(FILE_NAME ${file} NAME_WE)
    get_filename_component(FILE_EXT ${file} EXT)
    file(APPEND ${SHADER_LIB_SRC}
    "        { \"${FILE_NAME}${FILE_EXT}\", ${prefix}${FILE_NAME} },\n")
  endforeach()

  file(APPEND ${SHADER_LIB_SRC}
  "    };\n"
  "    auto res = lib.find(a_FileName);\n"
  "    return res != lib.end() ? res->second : \"\";\n"
  "}\n")
endfunction(GenerateFunction)

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

function(ParseJSONList a_JSONString a_ListName a_OutVar)
  string(JSON COUNT LENGTH ${a_JSONString} ${a_ListName})
  math(EXPR COUNT "${COUNT}-1")
  foreach(IDX RANGE "${COUNT}")
    string(JSON VAR GET ${a_JSONString} ${a_ListName} ${IDX})
    list(APPEND ${a_OutVar} ${VAR})
  endforeach(IDX)
  return(PROPAGATE ${a_OutVar})
endfunction()

function(ParseDefines a_JSONString a_OutVar)
  ParseJSONList(${a_JSONString} defines DEFINES_LIST)
  foreach(DEFINE ${DEFINES_LIST})
    string(JSON DEFINE_NAME GET ${DEFINE} name)
    string(JSON DEFINE_VALUE GET ${DEFINE} value)
    string(APPEND ${a_OutVar} "#define ${DEFINE_NAME} ${DEFINE_VALUE}\\n")
  endforeach(DEFINE ${DEFINES_LIST})
  return(PROPAGATE ${a_OutVar})
endfunction()

function(ParseStage a_JSONString a_GlobalDefines a_OutVar)
  set(CODE "")
  ParseDefines(${a_JSONString} STAGE_DEFINES)
  string(JSON ENTRY_POINT GET ${a_JSONString} entryPoint)
  string(JSON STAGE_FILE  GET ${a_JSONString} file)
  string(APPEND CODE ${${a_GlobalDefines}})
  string(APPEND CODE ${STAGE_DEFINES})
  if(CODE STREQUAL "")
    set(CODE "GetStage(${STAGE_FILE})")
  else()
    set(CODE "\"${CODE}\" + GetStage(${STAGE_FILE})")
  endif(CODE STREQUAL "")
  list(APPEND ${a_OutVar} "ProgramStage{ .entryPoint=${ENTRY_POINT}, .code=ShaderPreprocessor{}::ExpandCode(${CODE}) }")
  return(PROPAGATE ${a_OutVar})
endfunction()

foreach(file ${GLSL_PROGRAM_FILES})
  file(READ ${file} JSON_STRING)
  string(JSON CUR_NAME GET ${JSON_STRING} name)
  ParseDefines(${JSON_STRING} GLOBAL_DEFINES)
  ParseJSONList(${JSON_STRING} stages STAGES_LIST)
  foreach(STAGE_JSON ${STAGES_LIST})
    message("STAGE_JSON : ${STAGE_JSON}")
    ParseStage(${STAGE_JSON} GLOBAL_DEFINES STAGE_CODE)
    message("STAGE_CODE : ${STAGE_CODE}")
  endforeach()
  
  message("name : ${CUR_NAME}")
  message("Global defines : ${GLOBAL_DEFINES}")
  message("Stages : ${STAGES_LIST}")
endforeach(file ${GLSL_PROGRAM_FILES})
