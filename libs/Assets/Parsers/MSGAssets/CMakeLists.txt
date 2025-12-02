cmake_minimum_required(VERSION 3.19)

Fetch_NlohmannJson()

set(SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/MSGAssets.cpp)

add_library(MSG-Assets-Parsers-MSGAssets ${SOURCE_FILES})
target_include_directories(MSG-Assets-Parsers-MSGAssets PRIVATE
  "${GL_INCLUDE_DIRS}"
  "${EGL_INCLUDE_DIRS}")
target_link_libraries(MSG-Assets-Parsers-MSGAssets
  MSG::Assets
  MSG::Scene
  MSG::Buffer
  MSG::PrimitiveOptimizer
  MSG::TextureUtils
  MSG::ImageUtils
  MSG::Mesh
  MSG::Material
  MSG::Animation
  nlohmann_json)

set_target_properties(
  MSG-Assets-Parsers-MSGAssets
  PROPERTIES FOLDER "MSG/Assets/Parsers")

set(MSG_PARSER_LIBS ${MSG_PARSER_LIBS} MSG-Assets-Parsers-MSGAssets PARENT_SCOPE)

#Mime-Extension
set(MSG_PARSER_MIME_EXTENSION0 ${MSG_PARSER_MIME_EXTENSION0} "model/msgAssets+json" PARENT_SCOPE)
set(MSG_PARSER_MIME_EXTENSION1 ${MSG_PARSER_MIME_EXTENSION1} ".msgAssets" PARENT_SCOPE)

set(MSG_PARSER_MIME_FUNCTION0 ${MSG_PARSER_MIME_FUNCTION0} "model/msgAssets+json" PARENT_SCOPE)
set(MSG_PARSER_MIME_FUNCTION1 ${MSG_PARSER_MIME_FUNCTION1} "ParseMSGAssets" PARENT_SCOPE)