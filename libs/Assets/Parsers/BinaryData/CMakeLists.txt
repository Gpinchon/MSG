cmake_minimum_required(VERSION 3.19)

set(MSG_BINARY_DATA_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/BinaryData.cpp)

add_library(MSG-Assets-Parsers-BinaryData ${MSG_BINARY_DATA_SOURCE_FILES})
target_link_libraries(MSG-Assets-Parsers-BinaryData MSG::Assets MSG::Buffer)

set_target_properties(
  MSG-Assets-Parsers-BinaryData
  PROPERTIES FOLDER "MSG/Assets/Parsers")

set(MSG_PARSER_LIBS ${MSG_PARSER_LIBS} MSG-Assets-Parsers-BinaryData PARENT_SCOPE)
#Mime-Extension
set(MSG_PARSER_MIME_EXTENSION0 ${MSG_PARSER_MIME_EXTENSION0} "application/octet-stream" PARENT_SCOPE)
set(MSG_PARSER_MIME_EXTENSION1 ${MSG_PARSER_MIME_EXTENSION1} ".bin" PARENT_SCOPE)

set(MSG_PARSER_MIME_FUNCTION0 ${MSG_PARSER_MIME_FUNCTION0} "application/octet-stream" PARENT_SCOPE)
set(MSG_PARSER_MIME_FUNCTION1 ${MSG_PARSER_MIME_FUNCTION1} "ParseBinaryData" PARENT_SCOPE)