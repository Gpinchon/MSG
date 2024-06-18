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
  string(JSON STAGE_NAME GET ${a_JSONString} stage)
  string(JSON ENTRY_POINT GET ${a_JSONString} entryPoint)
  string(JSON STAGE_FILE  GET ${a_JSONString} file)
  string(APPEND CODE ${a_GlobalDefines})
  string(APPEND CODE ${STAGE_DEFINES})
  if(CODE STREQUAL "")
    set(CODE "GetStage(\"${STAGE_FILE}\")")
  else()
    set(CODE "\"${CODE}\" + GetStage(\"${STAGE_FILE}\")")
  endif(CODE STREQUAL "")
  list(APPEND ${a_OutVar} "ProgramStage{ .stage=\"${STAGE_NAME}\", .entryPoint=\"${ENTRY_POINT}\", .code=ShaderPreprocessor{}.ExpandCode(${CODE}) }")
  return(PROPAGATE ${a_OutVar})
endfunction()

function(ParseStages a_JSONString a_GlobalDefines a_OutVar)
  ParseJSONList(${a_JSONString} stages STAGES_LIST)
  foreach(STAGE_JSON ${STAGES_LIST})
    set(STAGE_CODE "")
    ParseStage(${STAGE_JSON} ${a_GlobalDefines} STAGE_CODE)
    list(APPEND ${a_OutVar} "${STAGE_CODE}")
  endforeach()
  list(JOIN ${a_OutVar} ", " ${a_OutVar})
  return(PROPAGATE ${a_OutVar})
endfunction()

set(PROGRAMS_CODE "")
foreach(file ${GLSL_PROGRAM_FILES})
  file(READ ${file} JSON_STRING)
  string(JSON NAME GET ${JSON_STRING} name)
  ParseDefines(${JSON_STRING} DEFINES)
  ParseStages(${JSON_STRING} ${DEFINES} STAGES)
  list(APPEND PROGRAMS_CODE "{ \"${NAME}\", Program{ .stages={ ${STAGES} } }}")
endforeach(file ${GLSL_PROGRAM_FILES})
list(JOIN ${PROGRAMS_CODE} ",\n" ${PROGRAMS_CODE})

file(APPEND ${SHADER_LIB_SRC} 
  "\nusing ProgramLibrary = std::unordered_map<std::string, TabGraph::Renderer::ShaderLibrary::Program>;\n")

file(APPEND ${SHADER_LIB_SRC} 
"\n"
"const TabGraph::Renderer::ShaderLibrary::Program& TabGraph::Renderer::ShaderLibrary::GetProgram(const std::string& a_Name) {\n"
"    static const Program emptyProgram;\n"
"    static const ProgramLibrary lib {\n")
file(APPEND ${SHADER_LIB_SRC} 
"        ${PROGRAMS_CODE}\n")
file(APPEND ${SHADER_LIB_SRC}
"    };\n"
"    auto res = lib.find(a_Name);\n"
"    return res != lib.end() ? res->second : emptyProgram;\n"
"}\n")