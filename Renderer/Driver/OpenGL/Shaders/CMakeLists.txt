set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/)
set(GENERATED_GLSL_DIR ${GENERATED_DIR}/GLSL)
set(SHADER_LIB_SRC ${GENERATED_DIR}/src/ShaderLibrary.cpp)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Generate_Shaders_Lib.cmake)

set(GLSL_HEADER_FILES ${GLSL_HEADER_FILES}
  ${INCLUDE_DIR}/GLSL/CameraUBO.glsl
  ${INCLUDE_DIR}/GLSL/MaterialUBO.glsl
  ${INCLUDE_DIR}/GLSL/TransformUBO.glsl
  ${INCLUDE_DIR}/GLSL/Types.glsl)

set(GLSL_STAGE_FILES ${GLSL_STAGE_FILES}
  ${CMAKE_CURRENT_SOURCE_DIR}/stage/ForwardFragment.glsl
  ${CMAKE_CURRENT_SOURCE_DIR}/stage/ForwardVertex.glsl
  ${CMAKE_CURRENT_SOURCE_DIR}/stage/SwapChainFragment.glsl
  ${CMAKE_CURRENT_SOURCE_DIR}/stage/SwapChainVertex.glsl)

set(HEADER_FILES
  ${INCLUDE_DIR}/Renderer/ShaderLibrary.hpp
  ${INCLUDE_DIR}/Renderer/ShaderPreprocessor.hpp)

set(SOURCE_FILES
  ${SHADER_LIB_SRC}
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ShaderPreprocessor.cpp)

add_library(TabGraph-Renderer-ShadersLibrary STATIC
  ${HEADER_FILES} ${SOURCE_FILES})
add_library(TabGraph::Renderer::ShadersLibrary ALIAS TabGraph-Renderer-ShadersLibrary)
target_include_directories(TabGraph-Renderer-ShadersLibrary PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include/
  ${GENERATED_DIR})

GenerateShaderLib()
