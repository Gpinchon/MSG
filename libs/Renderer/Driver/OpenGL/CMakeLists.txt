cmake_policy(SET CMP0072 NEW)

set(MSG_RENDERER_LIBS ${OPENGL_LIBRARIES})

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Renderer/OGL)
set(SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Renderer/OGL)

set(MSG_RENDERER_IMPL_HEADER_FILES
  ${MSG_RENDERER_IMPL_HEADER_FILES}
  ${INCLUDE_DIR}/Pass/PassInterface.hpp
  ${INCLUDE_DIR}/Pass/PassLibrary.hpp
  ${INCLUDE_DIR}/Subsystems/FrameSubsystem.hpp
  ${INCLUDE_DIR}/Subsystems/CameraSubsystem.hpp
  ${INCLUDE_DIR}/Subsystems/LightsSubsystem.hpp
  ${INCLUDE_DIR}/Subsystems/FogSubsystem.hpp
  ${INCLUDE_DIR}/Subsystems/TransformSubsystem.hpp
  ${INCLUDE_DIR}/Subsystems/MaterialSubsystem.hpp
  ${INCLUDE_DIR}/Subsystems/SkinSubsystem.hpp
  ${INCLUDE_DIR}/Subsystems/MeshSubsystem.hpp
  ${INCLUDE_DIR}/Subsystems/SubsystemInterface.hpp
  ${INCLUDE_DIR}/Subsystems/SubsystemLibrary.hpp
  ${INCLUDE_DIR}/Components/LightData.hpp
  ${INCLUDE_DIR}/Components/Mesh.hpp
  ${INCLUDE_DIR}/Components/MeshSkin.hpp
  ${INCLUDE_DIR}/Components/Transform.hpp
  ${INCLUDE_DIR}/Loader/MaterialLoader.hpp
  ${INCLUDE_DIR}/Loader/SamplerLoader.hpp
  ${INCLUDE_DIR}/Loader/TextureLoader.hpp
  ${INCLUDE_DIR}/WorleyPerlinNoise.hpp
  ${INCLUDE_DIR}/LightCullerVTFS.hpp
  ${INCLUDE_DIR}/Material.hpp
  ${INCLUDE_DIR}/Primitive.hpp
  ${INCLUDE_DIR}/RenderBuffer.hpp
  ${INCLUDE_DIR}/Renderer.hpp
  ${INCLUDE_DIR}/RendererPathFwd.hpp
  ${INCLUDE_DIR}/RendererPathDfd.hpp
  ${INCLUDE_DIR}/ShaderCompiler.hpp
  ${INCLUDE_DIR}/ToGL.hpp
  ${INCLUDE_DIR}/Vertex.hpp)

set(MSG_RENDERER_IMPL_SOURCE_FILES
  ${SOURCE_DIR}/Subsystems/FrameSubsystem.cpp
  ${SOURCE_DIR}/Subsystems/CameraSubsystem.cpp
  ${SOURCE_DIR}/Subsystems/LightsSubsystem.cpp
  ${SOURCE_DIR}/Subsystems/TransformSubsystem.cpp
  ${SOURCE_DIR}/Subsystems/MaterialSubsystem.cpp
  ${SOURCE_DIR}/Subsystems/SkinSubsystem.cpp
  ${SOURCE_DIR}/Subsystems/MeshSubsystem.cpp
  ${SOURCE_DIR}/Subsystems/FogSubsystem.cpp
  ${SOURCE_DIR}/Subsystems/SubsystemLibrary.cpp
  ${SOURCE_DIR}/Components/LightData.cpp
  ${SOURCE_DIR}/Components/MeshSkin.cpp
  ${SOURCE_DIR}/Loader/MaterialLoader.cpp
  ${SOURCE_DIR}/Loader/SamplerLoader.cpp
  ${SOURCE_DIR}/Loader/TextureLoader.cpp
  ${SOURCE_DIR}/WorleyPerlinNoise.cpp
  ${SOURCE_DIR}/LightCullerVTFS.cpp
  ${SOURCE_DIR}/Material.cpp
  ${SOURCE_DIR}/Primitive.cpp
  ${SOURCE_DIR}/RenderBuffer.cpp
  ${SOURCE_DIR}/Renderer.cpp
  ${SOURCE_DIR}/RendererPath.cpp
  ${SOURCE_DIR}/RendererPathFwd.cpp
  ${SOURCE_DIR}/RendererPathDfd.cpp
  ${SOURCE_DIR}/ShaderCompiler.cpp)

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
  MSG::BRDF
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
