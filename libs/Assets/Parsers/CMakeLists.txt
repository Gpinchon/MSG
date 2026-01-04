cmake_minimum_required(VERSION 3.19)

set(MSG_PARSER_LIBS ${MSG_PARSER_LIBS} PARENT_SCOPE)

add_subdirectory(BinaryData)
add_subdirectory(BMP)
add_subdirectory(BT)
add_subdirectory(FBX)
add_subdirectory(GLTF)
add_subdirectory(HDR)
add_subdirectory(KTX)
add_subdirectory(MTLLIB)
add_subdirectory(OBJ)
add_subdirectory(STBImage)
add_subdirectory(DDS)
add_subdirectory(MSGAssets)

set(INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
set(HEADER_PATH ${CMAKE_CURRENT_BINARY_DIR}/generated/MSG/Assets/Parsers.hpp)
set(SOURCE_PATH ${CMAKE_CURRENT_BINARY_DIR}/generated/MSG/Assets/Parsers.cpp)

file(WRITE ${HEADER_PATH}
"#pragma once
#include <memory>

namespace Msg::Assets {
class Asset;
void InitParsers();
//Forward declare parsing functions
")

foreach(function ${MSG_PARSER_MIME_FUNCTION1})
  file(APPEND ${HEADER_PATH} "std::shared_ptr<Asset> ${function}(const std::shared_ptr<Asset>&);\n")
endforeach()

file(APPEND ${HEADER_PATH} "}\n")

file(WRITE ${SOURCE_PATH}
"#include <MSG/Assets/Parsers.hpp>
#include <MSG/Assets/Parser.hpp>

#include <memory>

namespace Msg::Assets {
void InitParsers() {
")

list(LENGTH MSG_PARSER_MIME_EXTENSION0 MimeCount)
math(EXPR MimeCount "${MimeCount}-1")
foreach(i RANGE ${MimeCount})
  list(GET MSG_PARSER_MIME_EXTENSION0 ${i} Mime)
  list(GET MSG_PARSER_MIME_EXTENSION1 ${i} Extension)
  message(STATUS "Mime : ${Mime}; Extension : ${Extension};")
  file(APPEND ${SOURCE_PATH}
  "    Parser::AddMimeExtension(\"${Mime}\", \"${Extension}\");\n")
endforeach()

list(LENGTH MSG_PARSER_MIME_FUNCTION0 MimeCount)
math(EXPR MimeCount "${MimeCount}-1")
foreach(i RANGE ${MimeCount})
  list(GET MSG_PARSER_MIME_FUNCTION0 ${i} Mime)
  list(GET MSG_PARSER_MIME_FUNCTION1 ${i} Function)
  message(STATUS "Mime : ${Mime}; Function : ${Function};")
  file(APPEND ${SOURCE_PATH}
  "    Parser::Add(\"${Mime}\", ${Function});\n")
endforeach()

file(APPEND ${SOURCE_PATH}
"}
}
")

add_library(MSG-Assets-Parsers ${HEADER_PATH}  ${SOURCE_PATH})
add_library(MSG::Assets::Parsers ALIAS MSG-Assets-Parsers)
target_include_directories(MSG-Assets-Parsers PUBLIC "${INCLUDE_DIR}")
target_link_libraries(MSG-Assets-Parsers PUBLIC MSG-Assets ${MSG_PARSER_LIBS})

set_target_properties(
  MSG-Assets-Parsers
  PROPERTIES FOLDER "MSG/Assets/Parsers")
