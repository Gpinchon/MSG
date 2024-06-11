include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Fetch_Dependencies.cmake)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Shaders)

Fetch_GLEW()

set(TABGRAPH_RENDERER_LIBS
  GLEW::GLEW)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/Renderer/OGL)
set(SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer/OGL)

set(TABGRAPH_RENDERER_IMPL_HEADER_FILES
  ${TABGRAPH_RENDERER_IMPL_HEADER_FILES}
  ${INCLUDE_DIR}/Components/MeshData.hpp
  ${INCLUDE_DIR}/Components/Transform.hpp
  ${INCLUDE_DIR}/RAII/Buffer.hpp
  ${INCLUDE_DIR}/RAII/DebugGroup.hpp
  ${INCLUDE_DIR}/RAII/FrameBuffer.hpp
  ${INCLUDE_DIR}/RAII/Program.hpp
  ${INCLUDE_DIR}/RAII/ProgramPipeline.hpp
  ${INCLUDE_DIR}/RAII/Sampler.hpp
  ${INCLUDE_DIR}/RAII/Shader.hpp
  ${INCLUDE_DIR}/RAII/Texture.hpp
  ${INCLUDE_DIR}/RAII/TextureSampler.hpp
  ${INCLUDE_DIR}/RAII/VertexArray.hpp
  ${INCLUDE_DIR}/RAII/Wrapper.hpp
  ${INCLUDE_DIR}/Loader/MaterialLoader.hpp
  ${INCLUDE_DIR}/Loader/SamplerLoader.hpp
  ${INCLUDE_DIR}/Loader/TextureLoader.hpp
  ${INCLUDE_DIR}/Loader/TextureSamplerLoader.hpp
  ${INCLUDE_DIR}/IndexDescription.hpp
  ${INCLUDE_DIR}/Material.hpp
  ${INCLUDE_DIR}/Primitive.hpp
  ${INCLUDE_DIR}/RenderBuffer.hpp
  ${INCLUDE_DIR}/Renderer.hpp
  ${INCLUDE_DIR}/RenderPass.hpp
  ${INCLUDE_DIR}/ShaderCompiler.hpp
  ${INCLUDE_DIR}/UniformBuffer.hpp
  ${INCLUDE_DIR}/Vertex.hpp
  ${INCLUDE_DIR}/VertexAttributeDescription.hpp
  ${INCLUDE_DIR}/VertexBindingDescription.hpp)

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
  ${SOURCE_DIR}/RAII/TextureSampler.cpp
  ${SOURCE_DIR}/RAII/VertexArray.cpp
  ${SOURCE_DIR}/Loader/MaterialLoader.cpp
  ${SOURCE_DIR}/Loader/SamplerLoader.cpp
  ${SOURCE_DIR}/Loader/TextureLoader.cpp
  ${SOURCE_DIR}/Loader/TextureSamplerLoader.cpp
  ${SOURCE_DIR}/Material.cpp
  ${SOURCE_DIR}/Primitive.cpp
  ${SOURCE_DIR}/RenderBuffer.cpp
  ${SOURCE_DIR}/Renderer.cpp
  ${SOURCE_DIR}/RenderPass.cpp
  ${SOURCE_DIR}/ShaderCompiler.cpp
  ${SOURCE_DIR}/SwapChain.cpp
  ${SOURCE_DIR}/UniformBuffer.cpp)

if (WIN32)
  set(TABGRAPH_RENDERER_IMPL_HEADER_FILES ${TABGRAPH_RENDERER_IMPL_HEADER_FILES}
    ${INCLUDE_DIR}/Win32/Context.hpp
    ${INCLUDE_DIR}/Win32/Error.hpp
    ${INCLUDE_DIR}/Win32/SwapChain.hpp
    ${INCLUDE_DIR}/Win32/Window.hpp)

  set(TABGRAPH_RENDERER_IMPL_SOURCE_FILES ${TABGRAPH_RENDERER_IMPL_SOURCE_FILES}
    ${SOURCE_DIR}/Win32/Context.cpp
    ${SOURCE_DIR}/Win32/Error.cpp
    ${SOURCE_DIR}/Win32/SwapChain.cpp
    ${SOURCE_DIR}/Win32/Window.cpp)
elseif(UNIX)
  find_package(X11)
  set(TABGRAPH_RENDERER_LIBS ${TABGRAPH_RENDERER_LIBS}
    ${X11_LIBRARIES})
  find_package(OpenGL)
  set(TABGRAPH_RENDERER_LIBS ${TABGRAPH_RENDERER_LIBS}
    OpenGL::GL OpenGL::EGL)
  message(${TABGRAPH_RENDERER_LIBS})

  set(TABGRAPH_RENDERER_IMPL_HEADER_FILES ${TABGRAPH_RENDERER_IMPL_HEADER_FILES}
    ${INCLUDE_DIR}/Unix/Context.hpp
    ${INCLUDE_DIR}/Unix/SwapChain.hpp)

  set(TABGRAPH_RENDERER_IMPL_SOURCE_FILES ${TABGRAPH_RENDERER_IMPL_SOURCE_FILES}
    ${SOURCE_DIR}/Unix/Context.cpp
    ${SOURCE_DIR}/Unix/SwapChain.cpp)
endif(WIN32)

add_library(TabGraph-Renderer-Implementation STATIC
  ${TABGRAPH_RENDERER_IMPL_SOURCE_FILES}
  ${TABGRAPH_RENDERER_IMPL_HEADER_FILES})
add_library(TabGraph::Renderer::Implementation ALIAS TabGraph-Renderer-Implementation)
target_compile_definitions(TabGraph-Renderer-Implementation PUBLIC GLEW_NO_GLU)
target_include_directories(TabGraph-Renderer-Implementation PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(TabGraph-Renderer-Implementation
  TabGraph::SG
  TabGraph::Renderer::Interface
  TabGraph::Renderer::ShadersLibrary
  ${TABGRAPH_RENDERER_LIBS})

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
