include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Fetch_Dependencies.cmake)

Fetch_GLEW()

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/Renderer/OGL)
set(SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer/OGL)

set(TABGRAPH_RENDERER_IMPL_GLSL_FILES
  ${INCLUDE_DIR}/GLSL/CameraUBO.hpp
  ${INCLUDE_DIR}/GLSL/MaterialUBO.hpp
  ${INCLUDE_DIR}/GLSL/TransformUBO.hpp
  ${INCLUDE_DIR}/GLSL/Types.hpp)
set(TABGRAPH_RENDERER_IMPL_HEADER_FILES
  ${TABGRAPH_RENDERER_IMPL_GLSL_FILES}
  ${INCLUDE_DIR}/RAII/Buffer.hpp
  ${INCLUDE_DIR}/RAII/DebugGroup.hpp
  ${INCLUDE_DIR}/RAII/FrameBuffer.hpp
  ${INCLUDE_DIR}/RAII/Program.hpp
  ${INCLUDE_DIR}/RAII/ProgramPipeline.hpp
  ${INCLUDE_DIR}/RAII/Sampler.hpp
  ${INCLUDE_DIR}/RAII/Shader.hpp
  ${INCLUDE_DIR}/RAII/Texture.hpp
  ${INCLUDE_DIR}/RAII/VertexArray.hpp
  ${INCLUDE_DIR}/RAII/Wrapper.hpp
  ${INCLUDE_DIR}/IndexDescription.hpp
  ${INCLUDE_DIR}/Primitive.hpp
  ${INCLUDE_DIR}/RenderBuffer.hpp
  ${INCLUDE_DIR}/Renderer.hpp
  ${INCLUDE_DIR}/RenderPass.hpp
  ${INCLUDE_DIR}/ShaderCompiler.hpp
  ${INCLUDE_DIR}/Vertex.hpp
  ${INCLUDE_DIR}/VertexAttributeDescription.hpp
  ${INCLUDE_DIR}/VertexBindingDescription.hpp
)

set(TABGRAPH_RENDERER_IMPL_SOURCE_FILES
  ${SOURCE_DIR}/Components/MeshData.cpp
  ${SOURCE_DIR}/RAII/Buffer.cpp
  ${SOURCE_DIR}/RAII/DebugGroup.cpp
  ${SOURCE_DIR}/RAII/FrameBuffer.cpp
  ${SOURCE_DIR}/RAII/Program.cpp
  ${SOURCE_DIR}/RAII/ProgramPipeline.cpp
  ${SOURCE_DIR}/RAII/Sampler.cpp
  ${SOURCE_DIR}/RAII/Shader.cpp
  ${SOURCE_DIR}/RAII/Texture.cpp
  ${SOURCE_DIR}/RAII/VertexArray.cpp
  ${SOURCE_DIR}/Primitive.cpp
  ${SOURCE_DIR}/RenderBuffer.cpp
  ${SOURCE_DIR}/Renderer.cpp
  ${SOURCE_DIR}/RenderPass.cpp
  ${SOURCE_DIR}/ShaderCompiler.cpp
  ${SOURCE_DIR}/SwapChain.cpp
)

if (WIN32)
set(TABGRAPH_RENDERER_IMPL_HEADER_FILES ${TABGRAPH_RENDERER_IMPL_HEADER_FILES}
  ${INCLUDE_DIR}/Win32/Context.hpp
  ${INCLUDE_DIR}/Win32/Error.hpp
  ${INCLUDE_DIR}/Win32/SwapChain.hpp
  ${INCLUDE_DIR}/Win32/Window.hpp
)

set(TABGRAPH_RENDERER_IMPL_SOURCE_FILES ${TABGRAPH_RENDERER_IMPL_SOURCE_FILES}
  ${SOURCE_DIR}/Win32/Context.cpp
  ${SOURCE_DIR}/Win32/Error.cpp
  ${SOURCE_DIR}/Win32/SwapChain.cpp
  ${SOURCE_DIR}/Win32/Window.cpp
)
endif(WIN32)

set(TABGRAPH_RENDERER_IMPL_GENERATED_PATH ${CMAKE_CURRENT_BINARY_DIR}/generated/)
set(TABGRAPH_RENDERER_IMPL_GLSL_LIB_SOURCE_FILE ${TABGRAPH_RENDERER_IMPL_GENERATED_PATH}/src/ShaderLibrary.cpp)

set(TABGRAPH_RENDERER_IMPL_GLSL_DIR ${TABGRAPH_RENDERER_IMPL_GENERATED_PATH}/GLSL)

file(WRITE ${TABGRAPH_RENDERER_IMPL_GLSL_LIB_SOURCE_FILE}
"//This generates the default shader libary
#include <string>
#include <unordered_map>
")

function(make_includable input_file output_file)
    file(READ ${input_file} content)
    set(delim "for_c++_include")
    set(content "R\"${delim}(\n${content})${delim}\"")
    file(WRITE ${output_file} "${content}")
endfunction(make_includable)

foreach(header_file ${TABGRAPH_RENDERER_IMPL_GLSL_FILES})
  get_filename_component(FILE_NAME ${header_file} NAME_WE)
  make_includable(
    ${header_file}
    ${TABGRAPH_RENDERER_IMPL_GLSL_DIR}/${FILE_NAME}.glsl)
  file(APPEND ${TABGRAPH_RENDERER_IMPL_GLSL_LIB_SOURCE_FILE}
"auto ${FILE_NAME} =
    #include <GLSL/${FILE_NAME}.glsl>
;
")
endforeach()

file(APPEND ${TABGRAPH_RENDERER_IMPL_GLSL_LIB_SOURCE_FILE} 
"namespace TabGraph::Renderer::ShaderLibrary {
using Library = std::unordered_map<std::string, std::string>;
std::string GetCode(const std::string& a_IncludeName) {
    static Library lib {
")

foreach(header_file ${TABGRAPH_RENDERER_IMPL_GLSL_FILES})
  get_filename_component(FILE_NAME ${header_file} NAME_WE)
  make_includable(
    ${header_file}
    ${TABGRAPH_RENDERER_IMPL_GLSL_DIR}/${FILE_NAME}.glsl)
  
  file(APPEND ${TABGRAPH_RENDERER_IMPL_GLSL_LIB_SOURCE_FILE}
"        { \"${FILE_NAME}.glsl\", ${FILE_NAME} },
")
endforeach()

file(APPEND ${TABGRAPH_RENDERER_IMPL_GLSL_LIB_SOURCE_FILE}
"    };
    auto res = lib.find(a_IncludeName);
    return res != lib.end() ? res->second : \"\";
}
}")


add_library(TabGraph-Renderer-Implementation
  ${TABGRAPH_RENDERER_IMPL_SOURCE_FILES}
  ${TABGRAPH_RENDERER_IMPL_HEADER_FILES}
  ${TABGRAPH_RENDERER_IMPL_GLSL_LIB_SOURCE_FILE}
  ${TABGRAPH_RENDERER_IMPL_GLSL_LIB_HEADER_FILE})
add_library(TabGraph::Renderer::Implementation ALIAS TabGraph-Renderer-Implementation)
target_include_directories(TabGraph-Renderer-Implementation PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  ${TABGRAPH_RENDERER_IMPL_GENERATED_PATH})
target_link_libraries(TabGraph-Renderer-Implementation
  TabGraph::SG
  TabGraph::Renderer::Interface
  GLEW::GLEW)

set_target_properties(
  TabGraph-Renderer-Implementation
  PROPERTIES FOLDER "TabGraph"
)

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()
