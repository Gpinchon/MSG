cmake_minimum_required(VERSION 3.19)

set(TABGRAPH_PARSER_LIBS ${TABGRAPH_PARSER_LIBS} PARENT_SCOPE)

add_subdirectory(BinaryData)
add_subdirectory(BMP)
add_subdirectory(BT)
add_subdirectory(FBX)
add_subdirectory(GLTF)
add_subdirectory(HDR)
add_subdirectory(KTX)
add_subdirectory(OBJ)
add_subdirectory(STBImage)

set(INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
set(HEADER_PATH ${CMAKE_CURRENT_BINARY_DIR}/generated/Assets/Parsers.hpp)
set(SOURCE_PATH ${CMAKE_CURRENT_BINARY_DIR}/generated/Assets/Parsers.cpp)

file(WRITE ${HEADER_PATH}
"#pragma once
#include <memory>

namespace TabGraph::Assets {
class Asset;
void InitParsers();
//Forward declare parsing functions
")

foreach(function ${TABGRAPH_PARSER_MIME_FUNCTION1})
  file(APPEND ${HEADER_PATH} "std::shared_ptr<Asset> ${function}(const std::shared_ptr<Asset>&);\n")
endforeach()

file(APPEND ${HEADER_PATH} "}\n")

file(WRITE ${SOURCE_PATH}
"#include <Assets/Parsers.hpp>
#include <Assets/Parser.hpp>

#include <memory>

namespace TabGraph::Assets {
void InitParsers() {
")

list(LENGTH TABGRAPH_PARSER_MIME_EXTENSION0 MimeCount)
math(EXPR MimeCount "${MimeCount}-1")
foreach(i RANGE ${MimeCount})
  list(GET TABGRAPH_PARSER_MIME_EXTENSION0 ${i} Mime)
  list(GET TABGRAPH_PARSER_MIME_EXTENSION1 ${i} Extension)
  message(STATUS "Mime : ${Mime}; Extension : ${Extension};")
  file(APPEND ${SOURCE_PATH}
  "    Parser::AddMimeExtension(\"${Mime}\", \"${Extension}\");\n")
endforeach()

list(LENGTH TABGRAPH_PARSER_MIME_FUNCTION0 MimeCount)
math(EXPR MimeCount "${MimeCount}-1")
foreach(i RANGE ${MimeCount})
  list(GET TABGRAPH_PARSER_MIME_FUNCTION0 ${i} Mime)
  list(GET TABGRAPH_PARSER_MIME_FUNCTION1 ${i} Function)
  message(STATUS "Mime : ${Mime}; Function : ${Function};")
  file(APPEND ${SOURCE_PATH}
  "    Parser::Add(\"${Mime}\", ${Function});\n")
endforeach()

file(APPEND ${SOURCE_PATH}
"}
}
")

add_library(TabGraph-Assets-Parsers ${HEADER_PATH}  ${SOURCE_PATH})
add_library(TabGraph::Assets::Parsers ALIAS TabGraph-Assets-Parsers)
target_include_directories(TabGraph-Assets-Parsers PUBLIC "${INCLUDE_DIR}")
target_link_libraries(TabGraph-Assets-Parsers PUBLIC TabGraph-Assets ${TABGRAPH_PARSER_LIBS})

set_target_properties(
  TabGraph-Assets-Parsers
  PROPERTIES FOLDER "TabGraph/Assets/Parsers"
)
