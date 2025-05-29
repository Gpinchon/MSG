set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_source_files_properties(${SHADER_LIB_SRC} PROPERTIES GENERATED 1)

set(HEADER_FILES
  ${INCLUDE_DIR}/MSG/Renderer/ShaderLibrary.hpp
  ${INCLUDE_DIR}/MSG/Renderer/ShaderPreprocessor.hpp)

set(SOURCE_FILES
  ${SHADER_LIB_SRC}
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/ShaderPreprocessor.cpp)

add_library(MSG-Renderer-ShadersLibrary STATIC
  ${HEADER_FILES} ${SOURCE_FILES})
set_target_properties(
  MSG-Renderer-ShadersLibrary
  PROPERTIES FOLDER "MSG")
set_target_properties(
  GeneratedShaderLib
  PROPERTIES FOLDER "MSG")

add_library(MSG::Renderer::ShadersLibrary ALIAS MSG-Renderer-ShadersLibrary)
add_dependencies(MSG-Renderer-ShadersLibrary GeneratedShaderLib)
target_include_directories(MSG-Renderer-ShadersLibrary PUBLIC
  ${INCLUDE_DIR}
  ${SHADER_LIB_GENERATED_DIR})
  