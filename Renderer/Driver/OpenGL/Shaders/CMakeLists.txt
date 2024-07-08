set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/)
set(SHADER_LIB_SRC ${GENERATED_DIR}/src/ShaderLibrary.cpp)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(GLSL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/GLSL)
set(GLSL_HEADER_DIR ${GLSL_DIR}/header)
set(GLSL_STAGE_DIR ${GLSL_DIR}/stage/)
set(GLSL_PROGRAM_DIR ${GLSL_DIR}/program/)

set(GLSL_PROGRAM_FILES ${GLSL_PROGRAM_FILES}
  ${GLSL_PROGRAM_DIR}/Forward.json
  ${GLSL_PROGRAM_DIR}/SwapChain.json
  ${GLSL_PROGRAM_DIR}/VTFSCulling.json)

set(GLSL_HEADER_FILES ${GLSL_HEADER_FILES}
${GLSL_HEADER_DIR}/Bindings.glsl
  ${GLSL_HEADER_DIR}/Camera.glsl
  ${GLSL_HEADER_DIR}/Functions.glsl
  ${GLSL_HEADER_DIR}/Lights.glsl
  ${GLSL_HEADER_DIR}/Material.glsl
  ${GLSL_HEADER_DIR}/Random.glsl
  ${GLSL_HEADER_DIR}/SampleShadowMap.glsl
  ${GLSL_HEADER_DIR}/Transform.glsl
  ${GLSL_HEADER_DIR}/Types.glsl
  ${GLSL_HEADER_DIR}/VTFS.glsl
  ${GLSL_HEADER_DIR}/VTFSInputs.glsl
  ${GLSL_HEADER_DIR}/VTFSLightSampling.glsl)

set(GLSL_STAGE_FILES ${GLSL_STAGE_FILES}
  ${GLSL_STAGE_DIR}/Forward.frag
  ${GLSL_STAGE_DIR}/Forward.vert
  ${GLSL_STAGE_DIR}/ForwardLit.frag
  ${GLSL_STAGE_DIR}/SwapChain.frag
  ${GLSL_STAGE_DIR}/SwapChain.vert
  ${GLSL_STAGE_DIR}/VTFSCulling.comp)

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
  ${GLSL_HEADER_DIR}
  ${GENERATED_DIR})
