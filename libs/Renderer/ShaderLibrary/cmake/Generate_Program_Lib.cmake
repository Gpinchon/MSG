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

function(ParseKeywords a_JSONString a_OutVar)
  set(${a_OutVar} "")
  ParseJSONList(${a_JSONString} keywords KEYWORDS_LIST)
  foreach(KEYWORD ${KEYWORDS_LIST})
    ParseJSONList(${KEYWORD} values KEYWORD_VALUES_LIST)
    set(KEYWORD_VALUES "")
    foreach(KEYWORD_VALUE ${KEYWORD_VALUES_LIST})
      list(APPEND KEYWORD_VALUES "\"${KEYWORD_VALUE}\"")
    endforeach(KEYWORD_VALUE ${KEYWORD_VALUES_LIST})
    list(JOIN KEYWORD_VALUES ", " KEYWORD_VALUES)
    string(JSON KEYWORD_NAME  GET ${KEYWORD} name)
    string(APPEND ${a_OutVar} "        { .name = \"${KEYWORD_NAME}\", .values = { ${KEYWORD_VALUES} } },\n")
  endforeach(KEYWORD ${KEYWORDS_LIST})
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
  "namespace MSG::Renderer::ShaderLibrary {\n"
  "struct IntermediateKeyword {\n"
  "  std::string name;\n"
  "  std::vector<std::string> values;\n"
  "};\n"
  "\n"
  "void CartesianRecurse(\n"
  "    std::vector<std::vector<std::string>>& accum,\n"
  "    std::vector<std::string> stack,\n"
  "    std::vector<std::vector<std::string>> sequences, int index)\n"
  "{\n"
  "    std::vector<std::string> sequence = sequences[index];\n"
  "    for (auto& i : sequence) {\n"
  "        stack.push_back(i);\n"
  "        if (index == 0)\n"
  "            accum.push_back(stack);\n"
  "        else\n"
  "            CartesianRecurse(accum, stack, sequences, index - 1);\n"
  "        stack.pop_back();\n"
  "    }\n"
  "}\n"
  "\n"
  "std::vector<std::vector<std::string>> CartesianProduct(const std::vector<std::vector<std::string>>& a_Sequences)\n"
  "{\n"
  "    std::vector<std::vector<std::string>> accum;\n"
  "    std::vector<std::string> stack;\n"
  "    if (!a_Sequences.empty())\n"
  "        CartesianRecurse(accum, stack, a_Sequences, a_Sequences.size() - 1);\n"
  "    for (auto& keys : accum)\n"
  "        std::reverse(keys.begin(), keys.end());\n"
  "    return accum;\n"
  "}\n"
  "\n"
  "std::vector<std::vector<std::pair<std::string, std::string>>> GenerateKeywords(const std::vector<IntermediateKeyword>& a_Keywords)\n"
  "{\n"
  "    std::vector<std::vector<std::string>> keys;\n"
  "    for (auto& keyword : a_Keywords)\n"
  "        auto& thisDefines = keys.emplace_back(keyword.values);\n"
  "    std::vector<std::vector<std::pair<std::string, std::string>>> result;\n"
  "    for (auto& values : CartesianProduct(keys)) {\n"
  "        result.emplace_back().resize(values.size());\n"
  "        for (size_t i = 0; i < values.size(); i++)\n"
  "            result.back().at(i) = { a_Keywords.at(i).name, values.at(i) };\n"
  "    }\n"
  "    return result;\n"
  "}\n"
  "\n")

  foreach(file ${a_ProgramFiles})
    file(READ ${file} JSON_STRING)
    string(JSON NAME GET ${JSON_STRING} name)
    message(Generate shader program : ${NAME})
    ParseKeywords(${JSON_STRING} KEYWORDS)
    ParseDefines(${JSON_STRING} DEFINES)
    ParseStages(${JSON_STRING} "${DEFINES}" STAGES)
    string(APPEND ${a_OutVar}
    "std::vector<Program> Generate${NAME}()\n"
    "{\n")
    if (KEYWORDS STREQUAL "")
      string(APPEND ${a_OutVar}
      "    std::string keywordDefines;\n"
      "    return { Program { .stages { ${STAGES} } } };\n"
      "}\n")
    else()
      string(APPEND ${a_OutVar}
      "    std::vector<Program> programs;\n"
      "    const std::vector<IntermediateKeyword> keywords = {\n"
      "${KEYWORDS}"
      "    };\n"
      "    for (auto& iKeywords : GenerateKeywords(keywords)) {\n"
      "        std::string keywordDefines;\n"
      "        for (auto& keyword : iKeywords)\n"
      "            keywordDefines += \"#define \" + keyword.first + \" \" + keyword.second + \"\\n\";\n"
      "        auto& program    = programs.emplace_back();\n"
      "        program.keywords = { iKeywords.begin(), iKeywords.end() };\n"
      "        program.stages = { ${STAGES} };\n"
      "    }\n"
      "    return programs;\n"
      "}\n")
    endif (KEYWORDS STREQUAL "")
  endforeach()

  string(APPEND ${a_OutVar}
  "\n"
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
  "}\n")
  return(PROPAGATE ${a_OutVar})
endfunction()
