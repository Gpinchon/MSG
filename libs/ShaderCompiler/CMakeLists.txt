Fetch_GoogleTest()
Fetch_GLSLang()
Fetch_SPIRVHeaders()
Fetch_SPIRVCross()
Fetch_SPIRVTools()

set(HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/ShaderCompiler.hpp)
set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/ShaderCompiler.cpp)

add_library(MSG-ShaderCompiler STATIC ${HEADER_FILES} ${SOURCE_FILES})
add_library(MSG::ShaderCompiler ALIAS MSG-ShaderCompiler)
target_include_directories(MSG-ShaderCompiler PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-ShaderCompiler
  MSG::Debug
  glslang
  SPIRV
  glslang-default-resource-limits
  spirv-cross-core
  spirv-cross-glsl
  spirv-cross-reflect
  spirv-tools)

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()