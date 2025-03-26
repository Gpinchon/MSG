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

function(ParseKeywordsNames a_JSONString a_OutVar)
  ParseJSONList(${a_JSONString} keywords KEYWORDS_LIST)
  set(KEYWORD_NAMES "")
  foreach(KEYWORD ${KEYWORDS_LIST})
    string(JSON KEYWORD_NAME_LIST GET ${KEYWORD} name)
    foreach(KEYWORD_NAME ${KEYWORD_NAME_LIST})
      list(APPEND KEYWORD_NAMES "\"${KEYWORD_NAME}\"")
    endforeach(KEYWORD_NAME ${KEYWORD_NAME_LIST})
  endforeach(KEYWORD ${KEYWORDS_LIST})
  list(JOIN KEYWORD_NAMES ", " KEYWORD_NAMES)
  set(${a_OutVar} "std::vector<std::string>{ ${KEYWORD_NAMES} }")
  return(PROPAGATE ${a_OutVar})
endfunction()

function(ParseKeywordsValues a_JSONString a_OutVar)
  ParseJSONList(${a_JSONString} keywords KEYWORDS_LIST)
  set(KEYWORDS_VALUES "")
  foreach(KEYWORD ${KEYWORDS_LIST})
    ParseJSONList(${KEYWORD} values KEYWORD_VALUES_LIST)
    set(KEYWORD_VALUES "")
    foreach(KEYWORD_VALUE ${KEYWORD_VALUES_LIST})
      list(APPEND KEYWORD_VALUES "\"${KEYWORD_VALUE}\"")
    endforeach(KEYWORD_VALUE ${KEYWORD_VALUES_LIST})
    list(JOIN KEYWORD_VALUES ", " KEYWORD_VALUES)
    list(APPEND KEYWORDS_VALUES "std::vector<std::string>{ ${KEYWORD_VALUES} }")
  endforeach(KEYWORD ${KEYWORDS_LIST})
  list(JOIN KEYWORDS_VALUES ", " KEYWORDS_VALUES)
  set(${a_OutVar} "${KEYWORDS_VALUES}")
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
  string(APPEND CODE ${STAGE_EXT})
  string(APPEND CODE "\" + keywordDefines + \"")
  string(APPEND CODE ${a_GlobalDefines})
  string(APPEND CODE ${STAGE_DEFINES})
  if(NOT ${entryPoint} STREQUAL "main")
    string(APPEND CODE "#define ${ENTRY_POINT} main\\n")
  endif()
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
    ParseStage(${STAGE_JSON} "${a_GlobalDefines}" STAGE_CODE)
    list(APPEND ${a_OutVar} "${STAGE_CODE}")
  endforeach()
  list(JOIN ${a_OutVar} ", " ${a_OutVar})
  return(PROPAGATE ${a_OutVar})
endfunction()

function(GeneratePrograms a_ProgramFiles a_OutVar)
  string(APPEND ${a_OutVar}
  "namespace MSG::Renderer::ShaderLibrary {\n")
  foreach(file ${a_ProgramFiles})
    file(READ ${file} JSON_STRING)
    string(JSON NAME GET ${JSON_STRING} name)
    message(Generate shader program : ${NAME})
    ParseKeywordsNames(${JSON_STRING} KEYWORD_NAMES)
    ParseKeywordsValues(${JSON_STRING} KEYWORD_VALUES)
    ParseDefines(${JSON_STRING} DEFINES)
    ParseStages(${JSON_STRING} "${DEFINES}" STAGES)
    string(APPEND ${a_OutVar}
    "std::vector<Program> Generate${NAME}()\n"
    "{\n")
    if (KEYWORD_VALUES STREQUAL "")
      string(APPEND ${a_OutVar}
      "    std::string keywordDefines;\n"
      "    return { Program { .stages { ${STAGES} } } };\n"
      "}\n")
    else()
      string(APPEND ${a_OutVar}
      "    std::vector<Program> programs;\n"
      "    const auto names  = ${KEYWORD_NAMES};\n"
      "    const auto values = std::views::cartesian_product(${KEYWORD_VALUES});\n"
      "    for (const auto& iValues : values) {\n"
      "        std::string keywordDefines;\n"
      "        auto& program = programs.emplace_back();\n"
      "        auto name     = names.begin();\n"
      "        std::apply([&name, &keywordDefines, &program](const auto&... a_Values) mutable {\n"
      "            auto expandValue = [&name, &keywordDefines, &program](const auto& a_Value) mutable {\n"
      "                keywordDefines += \"#define \" + *name + \" \" + a_Value + \"\\n\";\n"
      "                program.keywords.emplace_back(*name, a_Value);\n"
      "                name++;\n"
      "            };\n"
      "            (expandValue(a_Values), ...);\n"
      "        },\n"
      "            iValues);\n"
      "        program.stages = { ${STAGES} };\n"
      "    }\n"
      "    return programs;\n"
      "}\n")
    endif (KEYWORD_VALUES STREQUAL "")
  endforeach()

  string(APPEND ${a_OutVar}
  "const MSG::Renderer::ShaderLibrary::ProgramsLibrary& MSG::Renderer::ShaderLibrary::GetProgramsLibrary() {\n"
  "    static const ProgramsLibrary lib {\n")
  foreach(file ${a_ProgramFiles})
    file(READ ${file} JSON_STRING)
    string(JSON NAME GET ${JSON_STRING} name)
    string(APPEND ${a_OutVar}
    "        { \"${NAME}\", Generate${NAME}() },\n")
  endforeach()
  string(APPEND ${a_OutVar}
  "    };\n"
  "    return lib;\n"
  "}\n"
  "}")

  string(APPEND ${a_OutVar}
  "\n"
  "const MSG::Renderer::ShaderLibrary::Program& MSG::Renderer::ShaderLibrary::GetProgram(const std::string& a_Name, const ProgramKeywords& a_Keywords)\n"
  "{\n"
  "    static const Program emptyProgram;\n"
  "    auto& lib = GetProgramsLibrary();\n"
  "    auto res  = lib.find(a_Name);\n"
  "    if (res != lib.end()) {\n"
  "        for (auto& program : res->second) {\n"
  "            auto equal = program.keywords.size() == a_Keywords.size() && std::equal(program.keywords.begin(), program.keywords.end(), a_Keywords.begin());\n"
  "            if (equal)\n"
  "                return program;\n"
  "        }\n"
  "    }\n"
  "    std::cerr << \"Error: \" << __func__ << \" missing program \" << a_Name << \"\\n\";\n"
  "    return emptyProgram;\n"
  "}\n"
  "\n")
  return(PROPAGATE ${a_OutVar})
endfunction()
