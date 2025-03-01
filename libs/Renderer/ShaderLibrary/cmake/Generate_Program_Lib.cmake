function(ParseJSONList a_JSONString a_ListName a_OutVar)
  set(${a_OutVar} "")
  string(JSON COUNT ERROR_VARIABLE ErrorMessage LENGTH ${a_JSONString} ${a_ListName})
  if (NOT ${ErrorMessage} STREQUAL "NOTFOUND")
    message(STATUS "ParseJSONList:${ErrorMessage}")
    return(PROPAGATE ${a_OutVar})
  endif (NOT ${ErrorMessage} STREQUAL "NOTFOUND")
  math(EXPR COUNT "${COUNT}-1")
  foreach(IDX RANGE "${COUNT}")
    string(JSON VAR GET ${a_JSONString} ${a_ListName} ${IDX})
    list(APPEND ${a_OutVar} ${VAR})
  endforeach(IDX)
  return(PROPAGATE ${a_OutVar})
endfunction()

function(ParseDefines a_JSONString a_OutVar)
  set(${a_OutVar} "")
  ParseJSONList(${a_JSONString} defines DEFINES_LIST)
  foreach(DEFINE ${DEFINES_LIST})
    string(JSON DEFINE_NAME  GET ${DEFINE} name)
    string(JSON DEFINE_VALUE GET ${DEFINE} value)
    string(APPEND ${a_OutVar} "#define ${DEFINE_NAME} ${DEFINE_VALUE}\\n")
  endforeach(DEFINE ${DEFINES_LIST})
  return(PROPAGATE ${a_OutVar})
endfunction()

function(ParseExtensions a_JSONString a_OutVar)
  set(${a_OutVar} "")
  ParseJSONList(${a_JSONString} extensions EXT_LIST)
  foreach(EXT ${EXT_LIST})
    string(JSON EXT_NAME     GET ${EXT} name)
    string(JSON EXT_BEHAVIOR GET ${EXT} behavior)
    string(APPEND ${a_OutVar} "#extension ${EXT_NAME} : ${EXT_BEHAVIOR}\\n")
  endforeach(EXT ${EXT_LIST})
  return(PROPAGATE ${a_OutVar})
endfunction()

function(ParseStage a_JSONString a_GlobalDefines a_OutVar)
  set(${a_OutVar} "")
  set(CODE "")
  ParseExtensions(${a_JSONString} STAGE_EXT)
  ParseDefines(${a_JSONString} STAGE_DEFINES)
  string(JSON NAME GET ${a_JSONString} name)
  string(JSON VERSION GET ${a_JSONString} version)
  string(JSON ENTRY_POINT GET ${a_JSONString} entryPoint)
  string(JSON STAGE_FILE  GET ${a_JSONString} file)
  string(APPEND CODE "#version ${VERSION}\\n")
  if(NOT ${entryPoint} STREQUAL "main")
    string(APPEND CODE "#define ${ENTRY_POINT} main\\n")
  endif()
  string(APPEND CODE ${STAGE_EXT})
  string(APPEND CODE ${a_GlobalDefines})
  string(APPEND CODE ${STAGE_DEFINES})
  if(CODE STREQUAL "")
    set(CODE "GetStage(\"${STAGE_FILE}\")")
  else()
    set(CODE "\"${CODE}\" + GetStage(\"${STAGE_FILE}\")")
  endif(CODE STREQUAL "")
  list(APPEND ${a_OutVar} "ProgramStage{ .name=StageName::${NAME}, .entryPoint=\"${ENTRY_POINT}\", .code=ShaderPreprocessor{}.ExpandCode(${CODE}) }")
  return(PROPAGATE ${a_OutVar})
endfunction()

function(ParseStages a_JSONString a_GlobalDefines a_OutVar)
  set(${a_OutVar} "")
  ParseJSONList(${a_JSONString} stages STAGES_LIST)
  foreach(STAGE_JSON ${STAGES_LIST})
    set(STAGE_CODE "")
    ParseStage(${STAGE_JSON} ${a_GlobalDefines} STAGE_CODE)
    list(APPEND ${a_OutVar} "${STAGE_CODE}")
  endforeach()
  list(JOIN ${a_OutVar} ", " ${a_OutVar})
  return(PROPAGATE ${a_OutVar})
endfunction()

function(GeneratePrograms a_ProgramFiles a_OutVar)
string(APPEND ${a_OutVar}
  "const MSG::Renderer::ShaderLibrary::ProgramsLibrary& MSG::Renderer::ShaderLibrary::GetProgramsLibrary() {\n"
  "    static const ProgramsLibrary lib {\n")
  foreach(file ${a_ProgramFiles})
    file(READ ${file} JSON_STRING)
    string(JSON NAME GET ${JSON_STRING} name)
    message(Generate shader program : ${NAME})
    ParseDefines(${JSON_STRING} DEFINES)
    ParseStages(${JSON_STRING} ${DEFINES} STAGES)
    string(APPEND ${a_OutVar}
    "        { \"${NAME}\", Program{ .stages={ ${STAGES} } } },\n")
  endforeach()
  string(APPEND ${a_OutVar}
  "    };\n"
  "    return lib;\n"
  "}\n")
  string(APPEND ${a_OutVar}
  "\n"
  "const MSG::Renderer::ShaderLibrary::Program& MSG::Renderer::ShaderLibrary::GetProgram(const std::string& a_Name) {\n"
  "    static const Program emptyProgram;\n"
  "    auto& lib = GetProgramsLibrary();\n"
  "    auto res = lib.find(a_Name);\n"
  "    if (res != lib.end()) return res->second;\n"
  "    else {\n"
  "        std::cerr << \"Error: \" << __func__ <<\" missing program \" << a_Name << \"\\n\";\n"
  "        return emptyProgram;\n"
  "    }\n"
  "}\n")
  return(PROPAGATE ${a_OutVar})
endfunction()
