cmake_policy(SET CMP0072 NEW)

set(MSG_RENDERER_LIBS ${OPENGL_LIBRARIES})

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Renderer/OGL)
set(SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Renderer/OGL)

set(MSG_RENDERER_IMPL_HEADER_FILES
  ${MSG_RENDERER_IMPL_HEADER_FILES}
  ${INCLUDE_DIR}/Components/Mesh.hpp
  ${INCLUDE_DIR}/Components/MeshSkin.hpp
  ${INCLUDE_DIR}/Components/Transform.hpp
  ${INCLUDE_DIR}/Loader/MaterialLoader.hpp
  ${INCLUDE_DIR}/Loader/SamplerLoader.hpp
  ${INCLUDE_DIR}/Loader/TextureLoader.hpp
  ${INCLUDE_DIR}/LightCullerFwd.hpp
  ${INCLUDE_DIR}/LightCullerVTFS.hpp
  ${INCLUDE_DIR}/Material.hpp
  ${INCLUDE_DIR}/Primitive.hpp
  ${INCLUDE_DIR}/RenderBuffer.hpp
  ${INCLUDE_DIR}/Renderer.hpp
  ${INCLUDE_DIR}/RendererPathFwd.hpp
  ${INCLUDE_DIR}/ShaderCompiler.hpp
  ${INCLUDE_DIR}/ToGL.hpp
  ${INCLUDE_DIR}/UniformBuffer.hpp
  ${INCLUDE_DIR}/UniformBufferUpdate.hpp
  ${INCLUDE_DIR}/Vertex.hpp)

set(MSG_RENDERER_IMPL_SOURCE_FILES
  ${SOURCE_DIR}/Components/LightData.cpp
  ${SOURCE_DIR}/Components/MeshSkin.cpp
  ${SOURCE_DIR}/Loader/MaterialLoader.cpp
  ${SOURCE_DIR}/Loader/SamplerLoader.cpp
  ${SOURCE_DIR}/Loader/TextureLoader.cpp
  ${SOURCE_DIR}/LightCullerFwd.cpp
  ${SOURCE_DIR}/LightCullerVTFS.cpp
  ${SOURCE_DIR}/Material.cpp
  ${SOURCE_DIR}/Primitive.cpp
  ${SOURCE_DIR}/RenderBuffer.cpp
  ${SOURCE_DIR}/Renderer.cpp
  ${SOURCE_DIR}/RendererPathFwd.cpp
  ${SOURCE_DIR}/ShaderCompiler.cpp
  ${SOURCE_DIR}/UniformBuffer.cpp
  ${SOURCE_DIR}/UniformBufferUpdate.cpp)

add_library(MSG-Renderer-Implementation STATIC
  ${MSG_RENDERER_IMPL_SOURCE_FILES}
  ${MSG_RENDERER_IMPL_HEADER_FILES})
add_library(MSG::Renderer::Implementation ALIAS MSG-Renderer-Implementation)
target_include_directories(MSG-Renderer-Implementation PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-Renderer-Implementation
  MSG::Renderer::Interface
  MSG::Renderer::Shaders
  MSG::Renderer::ShadersLibrary
  MSG::OGL
  MSG::Scene
  ${MSG_RENDERER_LIBS})

set_target_properties(
  MSG-Renderer-Implementation
  PROPERTIES FOLDER "MSG/Renderer")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()
