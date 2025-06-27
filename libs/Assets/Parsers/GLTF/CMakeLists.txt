cmake_minimum_required(VERSION 3.19)

Fetch_NlohmannJson()
Fetch_EGLRegistry()
Fetch_OGLRegistry()

set(MSG_GLTF_DATA_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/GLTF.cpp)

add_library(MSG-Assets-Parsers-GLTF ${MSG_GLTF_DATA_SOURCE_FILES})
target_include_directories(MSG-Assets-Parsers-GLTF PRIVATE
  "${GL_INCLUDE_DIRS}"
  "${EGL_INCLUDE_DIRS}")
target_link_libraries(MSG-Assets-Parsers-GLTF
  MSG::Assets
  MSG::Buffer
  MSG::PrimitiveOptimizer
  MSG::TextureUtils
  MSG::ImageUtils
  nlohmann_json)

set_target_properties(
  MSG-Assets-Parsers-GLTF
  PROPERTIES FOLDER "MSG/Assets/Parsers")

set(MSG_PARSER_LIBS ${MSG_PARSER_LIBS} MSG-Assets-Parsers-GLTF PARENT_SCOPE)

#Mime-Extension
set(MSG_PARSER_MIME_EXTENSION0 ${MSG_PARSER_MIME_EXTENSION0} "model/gltf+json" PARENT_SCOPE)
set(MSG_PARSER_MIME_EXTENSION1 ${MSG_PARSER_MIME_EXTENSION1} ".gltf" PARENT_SCOPE)

set(MSG_PARSER_MIME_FUNCTION0 ${MSG_PARSER_MIME_FUNCTION0} "model/gltf+json" PARENT_SCOPE)
set(MSG_PARSER_MIME_FUNCTION1 ${MSG_PARSER_MIME_FUNCTION1} "ParseGLTF" PARENT_SCOPE)