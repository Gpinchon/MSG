set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/)
set(SHADER_LIB_SRC ${GENERATED_DIR}/ShaderLibrary.cpp)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(HEADER_FILES
  ${INCLUDE_DIR}/MSG/Renderer/ShaderLibrary.hpp
  ${INCLUDE_DIR}/MSG/Renderer/ShaderPreprocessor.hpp)

set(SOURCE_FILES
  ${SHADER_LIB_SRC}
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/ShaderPreprocessor.cpp)

add_custom_target(GeneratedShaderLib
  DEPENDS ${SHADER_LIB_SRC}
  COMMENT "Checking if shader library re-generation is necessary")
add_custom_command(OUTPUT ${SHADER_LIB_SRC}
  COMMAND ${CMAKE_COMMAND}
  ARGS
    "-DCMAKE_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
    "-DCMAKE_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}"
    "-DSHADERS_HEADER_FILES=${SHADERS_HEADER_FILES}"
    "-DSHADERS_STAGE_FILES=${SHADERS_STAGE_FILES}"
    "-DSHADERS_PROGRAM_FILES=${SHADERS_PROGRAM_FILES}"
    "-DSHADER_LIB_SRC=${SHADER_LIB_SRC}"
    "-DGENERATED_DIR=${GENERATED_DIR}"
    -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Generate_Shaders_Lib.cmake
  DEPENDS ${SHADERS_HEADER_FILES} ${SHADERS_STAGE_FILES} ${SHADERS_PROGRAM_FILES}
  VERBATIM
  COMMENT "Generating Shader Library")

add_library(MSG-Renderer-ShadersLibrary STATIC
  ${HEADER_FILES} ${SOURCE_FILES})
set_target_properties(
  MSG-Renderer-ShadersLibrary
  PROPERTIES FOLDER "MSG/Renderer")
set_target_properties(
  GeneratedShaderLib
  PROPERTIES FOLDER "MSG/Renderer")

add_library(MSG::Renderer::ShadersLibrary ALIAS MSG-Renderer-ShadersLibrary)
add_dependencies(MSG-Renderer-ShadersLibrary GeneratedShaderLib)
target_include_directories(MSG-Renderer-ShadersLibrary PUBLIC
  ${INCLUDE_DIR}
  ${GENERATED_DIR})

unset(SHADERS_HEADER_FILES CACHE)
unset(SHADERS_STAGE_FILES CACHE)
unset(SHADERS_PROGRAM_FILES CACHE)
