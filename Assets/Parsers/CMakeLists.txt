cmake_minimum_required(VERSION 3.19)

set(TABGRAPH_PARSER_LIBS ${TABGRAPH_PARSER_LIBS} PARENT_SCOPE)

add_subdirectory(BinaryData)
add_subdirectory(BMP)
add_subdirectory(FBX)
add_subdirectory(GLTF)

file(WRITE src/Parsers.cpp
"
#include <Assets/Asset.hpp>
#include <Assets/Parser.hpp>

#include <memory>

namespace TabGraph::Assets {
//Forward declare parsing functions
")


foreach(function ${TABGRAPH_PARSER_FUNCTIONS})
  file(APPEND src/Parsers.cpp
    "std::shared_ptr<Asset> ${function}(const std::shared_ptr<Asset>&);\n")
endforeach()

file(APPEND src/Parsers.cpp
"
void InitParsers() {
")

list(LENGTH TABGRAPH_PARSER_MIME_EXTENSION0 MimeCount)
math(EXPR MimeCount "${MimeCount}-1")
foreach(i RANGE ${MimeCount})
  list(GET TABGRAPH_PARSER_MIME_EXTENSION0 ${i} Mime)
  list(GET TABGRAPH_PARSER_MIME_EXTENSION1 ${i} Extension)
  message(STATUS "Mime : ${Mime}; Extension : ${Extension};")
  file(APPEND src/Parsers.cpp
  "    Parser::AddMimeExtension(\"${Mime}\", \"${Extension}\");\n")
endforeach()

list(LENGTH TABGRAPH_PARSER_MIME_FUNCTION0 MimeCount)
math(EXPR MimeCount "${MimeCount}-1")
foreach(i RANGE ${MimeCount})
  list(GET TABGRAPH_PARSER_MIME_FUNCTION0 ${i} Mime)
  list(GET TABGRAPH_PARSER_MIME_FUNCTION1 ${i} Function)
  message(STATUS "Mime : ${Mime}; Function : ${Function};")
  file(APPEND src/Parsers.cpp
  "    Parser::Add(\"${Mime}\", ${Function});\n")
endforeach()

file(APPEND src/Parsers.cpp
"}
}
")

add_library(TabGraph-Assets-Parsers ${CMAKE_CURRENT_SOURCE_DIR}/include/Assets/Parsers.hpp  ${CMAKE_CURRENT_SOURCE_DIR}/src/Parsers.cpp)
add_library(TabGraph::Assets::Parsers ALIAS TabGraph-Assets-Parsers)
target_include_directories(TabGraph-Assets-Parsers PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(TabGraph-Assets-Parsers PUBLIC TabGraph-Assets ${TABGRAPH_PARSER_LIBS})

set_target_properties(
  TabGraph-Assets-Parsers
  PROPERTIES FOLDER "TabGraph/Assets/Parsers"
)
