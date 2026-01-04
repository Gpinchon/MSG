cmake_minimum_required(VERSION 3.19)

set(MSG_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/DDS.cpp)

set(MSG_PRIVATE_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/private/D3D10ResourceDim.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/private/D3D10ResourceMiscFlag.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/private/DDSHeader.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/private/DDSPixelFlag.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/private/DXGIFormat.hpp)

add_library(MSG-Assets-Parsers-DDS ${MSG_SOURCE_FILES} ${MSG_PRIVATE_SOURCE_FILES})
target_include_directories(MSG-Assets-Parsers-DDS PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/private/)
target_link_libraries(MSG-Assets-Parsers-DDS
  MSG::Assets
  MSG::ImageUtils)

set_target_properties(
  MSG-Assets-Parsers-DDS
  PROPERTIES FOLDER "MSG/Assets/Parsers")

set(MSG_PARSER_LIBS ${MSG_PARSER_LIBS} MSG-Assets-Parsers-DDS PARENT_SCOPE)

set(MSG_PARSER_MIME_EXTENSION0 ${MSG_PARSER_MIME_EXTENSION0} "image/x-direct-draw-surface" PARENT_SCOPE)
set(MSG_PARSER_MIME_EXTENSION1 ${MSG_PARSER_MIME_EXTENSION1} ".dds" PARENT_SCOPE)

set(MSG_PARSER_MIME_FUNCTION0 ${MSG_PARSER_MIME_FUNCTION0} "image/x-direct-draw-surface" PARENT_SCOPE)
set(MSG_PARSER_MIME_FUNCTION1 ${MSG_PARSER_MIME_FUNCTION1} "ParseDDSImage" PARENT_SCOPE)
