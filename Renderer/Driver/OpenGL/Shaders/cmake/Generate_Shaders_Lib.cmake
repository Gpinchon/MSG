message(GLSL_HEADER_FILES : ${GLSL_HEADER_FILES})
message(GLSL_STAGE_FILES : ${GLSL_STAGE_FILES})
message(SHADER_LIB_SRC: ${SHADER_LIB_SRC})
message(GENERATED_GLSL_DIR: ${GENERATED_GLSL_DIR})

function(make_includable input_file output_file)
  file(READ ${input_file} content)
  set(delim "for_c++_include")
  set(content "R\"${delim}(\n${content})${delim}\"")
  file(WRITE ${output_file} "${content}")
endfunction(make_includable)

macro(GenerateGLSLFiles)
  foreach(header_file ${GLSL_HEADER_FILES})
    get_filename_component(FILE_NAME ${header_file} NAME_WE)
    make_includable(
        ${header_file}
        ${GENERATED_GLSL_DIR}/${FILE_NAME}_H.glsl)
    file(APPEND ${SHADER_LIB_SRC}
    "constexpr auto ${FILE_NAME}_H =\n"
    "    #include <GLSL/${FILE_NAME}_H.glsl>\n"
    ";\n")
  endforeach()

  foreach(source_file ${GLSL_STAGE_FILES})
    get_filename_component(FILE_NAME ${source_file} NAME_WE)
    make_includable(
        ${source_file}
        ${GENERATED_GLSL_DIR}/${FILE_NAME}_S.glsl)
    file(APPEND ${SHADER_LIB_SRC}
    "constexpr auto ${FILE_NAME}_S =\n"
    "    #include <GLSL/${FILE_NAME}_S.glsl>\n"
    ";\n")
  endforeach()
endMacro(GenerateGLSLFiles)


file(WRITE ${SHADER_LIB_SRC}
"//This generates the default shader libary\n"
"#include <string>\n"
"#include <unordered_map>\n"
"#include <Renderer/ShaderLibrary.hpp>\n"
"#include <Renderer/ShaderPreprocessor.hpp>\n\n")

GenerateGLSLFiles()

#GENERATE SHADER HEADER LIB
file(APPEND ${SHADER_LIB_SRC} 
"\nusing Library = std::unordered_map<std::string, std::string>;\n"
"std::string TabGraph::Renderer::ShaderLibrary::GetHeader(const std::string& a_IncludeName) {\n"
"    static Library lib {\n")

foreach(header_file ${GLSL_HEADER_FILES})
get_filename_component(FILE_NAME ${header_file} NAME_WE)
file(APPEND ${SHADER_LIB_SRC}
"        { \"${FILE_NAME}.glsl\", ${FILE_NAME}_H },\n")
endforeach()

file(APPEND ${SHADER_LIB_SRC}
"    };\n"
"    auto res = lib.find(a_IncludeName);\n"
"    return res != lib.end() ? res->second : \"\";\n";
"}\n\n")

#GENERATE SHADER STAGE LIB
file(APPEND ${SHADER_LIB_SRC} 
"std::string TabGraph::Renderer::ShaderLibrary::GetStage(const std::string& a_IncludeName) {\n"
"    static Library lib {\n")

foreach(stage_file ${GLSL_STAGE_FILES})
get_filename_component(FILE_NAME ${stage_file} NAME_WE)
file(APPEND ${SHADER_LIB_SRC}
"        { \"${FILE_NAME}.glsl\", ${FILE_NAME}_S },\n")
endforeach()

file(APPEND ${SHADER_LIB_SRC}
"    };\n"
"    auto res = lib.find(a_IncludeName);\n"
"    return res != lib.end() ? res->second : \"\";\n"
"}\n")
