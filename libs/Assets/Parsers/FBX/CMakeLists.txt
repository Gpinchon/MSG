cmake_minimum_required(VERSION 3.19)

Fetch_ZLib()

set(MSG_FBX_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/FBX/FBXDocument.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/FBX/FBXNode.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/FBX/FBXObject.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/FBX/FBXProperty.hpp)

set(MSG_FBX_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/FBX.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/FBXDocument.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/FBXNode.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/FBXObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/FBXProperty.cpp)

add_library(MSG-Assets-Parsers-FBX ${MSG_FBX_HEADER_FILES} ${MSG_FBX_SOURCE_FILES})
target_link_libraries(MSG-Assets-Parsers-FBX MSG::Assets)

set_target_properties(
  MSG-Assets-Parsers-FBX
  PROPERTIES FOLDER "MSG/Assets/Parsers"
)
target_include_directories(MSG-Assets-Parsers-FBX PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${ZLIB_INCLUDE_DIR}")
target_link_libraries(MSG-Assets-Parsers-FBX ZLIB::ZLIB)

set(MSG_PARSER_LIBS ${MSG_PARSER_LIBS} MSG-Assets-Parsers-FBX PARENT_SCOPE)

#Not standard mime but fuck it.
set(MSG_PARSER_MIME_EXTENSION0 ${MSG_PARSER_MIME_EXTENSION0} "model/fbx" PARENT_SCOPE)
set(MSG_PARSER_MIME_EXTENSION1 ${MSG_PARSER_MIME_EXTENSION1} ".fbx" PARENT_SCOPE)

set(MSG_PARSER_MIME_FUNCTION0 ${MSG_PARSER_MIME_FUNCTION0} "model/fbx" PARENT_SCOPE)
set(MSG_PARSER_MIME_FUNCTION1 ${MSG_PARSER_MIME_FUNCTION1} "ParseFBX" PARENT_SCOPE)
