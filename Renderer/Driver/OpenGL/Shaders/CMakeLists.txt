set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/)
set(SHADER_LIB_SRC ${GENERATED_DIR}/src/ShaderLibrary.cpp)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/)
set(STAGE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/stage/)
set(PROGRAM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/program/)

set(GLSL_PROGRAM_FILES ${GLSL_PROGRAM_FILES}
  ${PROGRAM_DIR}/Forward.json)

set(GLSL_HEADER_FILES ${GLSL_HEADER_FILES}
  ${INCLUDE_DIR}/CameraUBO.glsl
  ${INCLUDE_DIR}/Functions.glsl
  ${INCLUDE_DIR}/MaterialUBO.glsl
  ${INCLUDE_DIR}/Random.glsl
  ${INCLUDE_DIR}/SampleShadowMap.glsl
  ${INCLUDE_DIR}/TransformUBO.glsl
  ${INCLUDE_DIR}/Types.glsl)

set(GLSL_STAGE_FILES ${GLSL_STAGE_FILES}
  ${STAGE_DIR}/ForwardFragment.glsl
  ${STAGE_DIR}/ForwardVertex.glsl
  ${STAGE_DIR}/SwapChainFragment.glsl
  ${STAGE_DIR}/SwapChainVertex.glsl)

set(HEADER_FILES
  ${INCLUDE_DIR}/Renderer/ShaderLibrary.hpp
  ${INCLUDE_DIR}/Renderer/ShaderPreprocessor.hpp)

set(SOURCE_FILES
  ${SHADER_LIB_SRC}
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ShaderPreprocessor.cpp)

add_custom_target(GeneratedShaderLib
  DEPENDS ${SHADER_LIB_SRC}
  COMMENT "Checking if shader library re-generation is necessary")
add_custom_command(OUTPUT ${SHADER_LIB_SRC}
  COMMAND ${CMAKE_COMMAND}
  ARGS
    "-DCMAKE_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
    "-DCMAKE_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}"
    "-DGLSL_HEADER_FILES=${GLSL_HEADER_FILES}"
    "-DGLSL_STAGE_FILES=${GLSL_STAGE_FILES}"
    "-DGLSL_PROGRAM_FILES=${GLSL_PROGRAM_FILES}"
    "-DSHADER_LIB_SRC=${SHADER_LIB_SRC}"
    "-DGENERATED_DIR=${GENERATED_DIR}"
    -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Generate_Shaders_Lib.cmake
  DEPENDS ${GLSL_HEADER_FILES} ${GLSL_STAGE_FILES} ${GLSL_PROGRAM_FILES}
  VERBATIM
  COMMENT "Generating Shader Library")

add_library(TabGraph-Renderer-ShadersLibrary STATIC
  ${HEADER_FILES} ${SOURCE_FILES})
add_library(TabGraph::Renderer::ShadersLibrary ALIAS TabGraph-Renderer-ShadersLibrary)
add_dependencies(TabGraph-Renderer-ShadersLibrary GeneratedShaderLib)
target_include_directories(TabGraph-Renderer-ShadersLibrary PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include/
  ${GENERATED_DIR})
