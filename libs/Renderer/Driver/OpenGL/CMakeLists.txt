cmake_policy(SET CMP0072 NEW)

Fetch_GLEW()

set(MSG_RENDERER_LIBS ${OPENGL_LIBRARIES})

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Renderer/OGL)
set(SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Renderer/OGL)

set(MSG_RENDERER_IMPL_HEADER_FILES
  ${MSG_RENDERER_IMPL_HEADER_FILES}
  ${INCLUDE_DIR}/Components/Mesh.hpp
  ${INCLUDE_DIR}/Components/MeshSkin.hpp
  ${INCLUDE_DIR}/Components/Transform.hpp
  ${INCLUDE_DIR}/RAII/Buffer.hpp
  ${INCLUDE_DIR}/RAII/DebugGroup.hpp
  ${INCLUDE_DIR}/RAII/FrameBuffer.hpp
  ${INCLUDE_DIR}/RAII/Program.hpp
  ${INCLUDE_DIR}/RAII/Sampler.hpp
  ${INCLUDE_DIR}/RAII/Shader.hpp
  ${INCLUDE_DIR}/RAII/Texture.hpp
  ${INCLUDE_DIR}/RAII/VertexArray.hpp
  ${INCLUDE_DIR}/RAII/Wrapper.hpp
  ${INCLUDE_DIR}/Loader/MaterialLoader.hpp
  ${INCLUDE_DIR}/Loader/SamplerLoader.hpp
  ${INCLUDE_DIR}/Loader/TextureLoader.hpp
  ${INCLUDE_DIR}/Context.hpp
  ${INCLUDE_DIR}/CPULightCuller.hpp
  ${INCLUDE_DIR}/GPULightCuller.hpp
  ${INCLUDE_DIR}/IndexDescription.hpp
  ${INCLUDE_DIR}/Material.hpp
  ${INCLUDE_DIR}/Primitive.hpp
  ${INCLUDE_DIR}/RenderBuffer.hpp
  ${INCLUDE_DIR}/Renderer.hpp
  ${INCLUDE_DIR}/RendererPathFwd.hpp
  ${INCLUDE_DIR}/RenderPass.hpp
  ${INCLUDE_DIR}/RenderPassInfo.hpp
  ${INCLUDE_DIR}/ShaderCompiler.hpp
  ${INCLUDE_DIR}/ToGL.hpp
  ${INCLUDE_DIR}/UniformBuffer.hpp
  ${INCLUDE_DIR}/UniformBufferUpdate.hpp
  ${INCLUDE_DIR}/Vertex.hpp
  ${INCLUDE_DIR}/VertexAttributeDescription.hpp
  ${INCLUDE_DIR}/VertexBindingDescription.hpp)

set(MSG_RENDERER_IMPL_SOURCE_FILES
  ${SOURCE_DIR}/Components/LightData.cpp
  ${SOURCE_DIR}/Components/MeshSkin.cpp
  ${SOURCE_DIR}/RAII/Buffer.cpp
  ${SOURCE_DIR}/RAII/DebugGroup.cpp
  ${SOURCE_DIR}/RAII/FrameBuffer.cpp
  ${SOURCE_DIR}/RAII/Program.cpp
  ${SOURCE_DIR}/RAII/Sampler.cpp
  ${SOURCE_DIR}/RAII/Shader.cpp
  ${SOURCE_DIR}/RAII/Texture.cpp
  ${SOURCE_DIR}/RAII/VertexArray.cpp
  ${SOURCE_DIR}/Loader/MaterialLoader.cpp
  ${SOURCE_DIR}/Loader/SamplerLoader.cpp
  ${SOURCE_DIR}/Loader/TextureLoader.cpp
  ${SOURCE_DIR}/Context.cpp
  ${SOURCE_DIR}/CPULightCuller.cpp
  ${SOURCE_DIR}/GPULightCuller.cpp
  ${SOURCE_DIR}/Material.cpp
  ${SOURCE_DIR}/Primitive.cpp
  ${SOURCE_DIR}/RenderBuffer.cpp
  ${SOURCE_DIR}/Renderer.cpp
  ${SOURCE_DIR}/RendererPathFwd.cpp
  ${SOURCE_DIR}/RenderPass.cpp
  ${SOURCE_DIR}/ShaderCompiler.cpp
  ${SOURCE_DIR}/UniformBuffer.cpp
  ${SOURCE_DIR}/UniformBufferUpdate.cpp)

if (WIN32)
  set(MSG_RENDERER_IMPL_HEADER_FILES ${MSG_RENDERER_IMPL_HEADER_FILES}
    ${INCLUDE_DIR}/Win32/PlatformCtx.hpp  
    ${INCLUDE_DIR}/Win32/WGL.hpp
    ${INCLUDE_DIR}/Win32/Win32.hpp)
  set(MSG_RENDERER_IMPL_SOURCE_FILES ${MSG_RENDERER_IMPL_SOURCE_FILES}
    ${SOURCE_DIR}/Win32/PlatformCtx.cpp
    ${SOURCE_DIR}/Win32/WGL.cpp
    ${SOURCE_DIR}/Win32/Win32.cpp)
elseif(UNIX)
  find_package(X11)
  set(MSG_RENDERER_LIBS ${MSG_RENDERER_LIBS}
    ${X11_LIBRARIES})
  set(MSG_RENDERER_IMPL_HEADER_FILES ${MSG_RENDERER_IMPL_HEADER_FILES}
    ${INCLUDE_DIR}/Unix/Context.hpp)
  set(MSG_RENDERER_IMPL_SOURCE_FILES ${MSG_RENDERER_IMPL_SOURCE_FILES}
    ${SOURCE_DIR}/Unix/Context.cpp)
endif(WIN32)

add_library(MSG-Renderer-Implementation STATIC
  ${MSG_RENDERER_IMPL_SOURCE_FILES}
  ${MSG_RENDERER_IMPL_HEADER_FILES})
add_library(MSG::Renderer::Implementation ALIAS MSG-Renderer-Implementation)
target_compile_definitions(MSG-Renderer-Implementation PUBLIC GLEW_NO_GLU)
target_compile_definitions(MSG-Renderer-Implementation PUBLIC GLEW_STATIC)
target_include_directories(MSG-Renderer-Implementation PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-Renderer-Implementation
  MSG::Scene
  MSG::Renderer::Interface
  MSG::Renderer::Shaders
  MSG::Renderer::ShadersLibrary
  ${MSG_RENDERER_LIBS})

set_target_properties(
  MSG-Renderer-Implementation
  PROPERTIES FOLDER "MSG"
)

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()
