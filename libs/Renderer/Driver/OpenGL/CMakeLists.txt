cmake_policy(SET CMP0072 NEW)

set(MSG_RENDERER_LIBS ${OPENGL_LIBRARIES})

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Renderer/OGL)
set(SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Renderer/OGL)

set(MSG_RENDERER_IMPL_HEADER_FILES
  ${MSG_RENDERER_IMPL_HEADER_FILES}
  ${INCLUDE_DIR}/RenderPasses/PassOpaqueGeometry.hpp
    ${INCLUDE_DIR}/RenderPasses/SubPassSkybox.hpp
    ${INCLUDE_DIR}/RenderPasses/SubPassOpaqueGeometry.hpp

  ${INCLUDE_DIR}/RenderPasses/PassLight.hpp
    ${INCLUDE_DIR}/RenderPasses/SubPassVTFS.hpp
    ${INCLUDE_DIR}/RenderPasses/SubPassIBL.hpp
    ${INCLUDE_DIR}/RenderPasses/SubPassShadow.hpp
  
  ${INCLUDE_DIR}/RenderPasses/PassPostTreatment.hpp
    ${INCLUDE_DIR}/RenderPasses/SubPassSSAO.hpp
    ${INCLUDE_DIR}/RenderPasses/SubPassFog.hpp
  
  ${INCLUDE_DIR}/RenderPasses/PassBlendedGeometry.hpp
    ${INCLUDE_DIR}/RenderPasses/SubPassOITForward.hpp
    ${INCLUDE_DIR}/RenderPasses/SubPassOITCompositing.hpp

  ${INCLUDE_DIR}/RenderPasses/PassToneMapping.hpp

  ${INCLUDE_DIR}/RenderPasses/PassTAA.hpp
  
  ${INCLUDE_DIR}/RenderPasses/PassPresent.hpp

  ${INCLUDE_DIR}/Subsystems/FrameSubsystem.hpp
  ${INCLUDE_DIR}/Subsystems/CameraSubsystem.hpp
  ${INCLUDE_DIR}/Subsystems/LightsSubsystem.hpp
  ${INCLUDE_DIR}/Subsystems/FogSubsystem.hpp
  ${INCLUDE_DIR}/Subsystems/MaterialSubsystem.hpp
  ${INCLUDE_DIR}/Subsystems/SkinSubsystem.hpp
  ${INCLUDE_DIR}/Subsystems/MeshSubsystem.hpp
  ${INCLUDE_DIR}/Subsystems/TexturingSubsystem.hpp
  ${INCLUDE_DIR}/Components/LightData.hpp
  ${INCLUDE_DIR}/Components/MaterialSet.hpp
  ${INCLUDE_DIR}/Components/Mesh.hpp
  ${INCLUDE_DIR}/Components/MeshSkin.hpp
  ${INCLUDE_DIR}/Loader/SamplerLoader.hpp
  ${INCLUDE_DIR}/Loader/TextureLoader.hpp
  ${INCLUDE_DIR}/Loader/SparseTextureLoader.hpp
  ${INCLUDE_DIR}/TextureBlurHelper.hpp
  ${INCLUDE_DIR}/WorleyPerlinNoise.hpp
  ${INCLUDE_DIR}/Material.hpp
  ${INCLUDE_DIR}/Primitive.hpp
  ${INCLUDE_DIR}/RenderBuffer.hpp
  ${INCLUDE_DIR}/Renderer.hpp
  ${INCLUDE_DIR}/ShaderCompiler.hpp
  ${INCLUDE_DIR}/ToGL.hpp
  ${INCLUDE_DIR}/SparseTexture.hpp
  ${INCLUDE_DIR}/SparseTexturePageCache.hpp
  ${INCLUDE_DIR}/Vertex.hpp)

set(MSG_RENDERER_IMPL_SOURCE_FILES
  ${SOURCE_DIR}/RenderPasses/PassOpaqueGeometry.cpp
    ${SOURCE_DIR}/RenderPasses/SubPassSkybox.cpp
    ${SOURCE_DIR}/RenderPasses/SubPassOpaqueGeometry.cpp

  ${SOURCE_DIR}/RenderPasses/PassLight.cpp
    ${SOURCE_DIR}/RenderPasses/SubPassVTFS.cpp
    ${SOURCE_DIR}/RenderPasses/SubPassIBL.cpp
    ${SOURCE_DIR}/RenderPasses/SubPassShadow.cpp
  
  ${SOURCE_DIR}/RenderPasses/PassPostTreatment.cpp
    ${SOURCE_DIR}/RenderPasses/SubPassSSAO.cpp
    ${SOURCE_DIR}/RenderPasses/SubPassFog.cpp

  ${SOURCE_DIR}/RenderPasses/PassBlendedGeometry.cpp
    ${SOURCE_DIR}/RenderPasses/SubPassOITForward.cpp
    ${SOURCE_DIR}/RenderPasses/SubPassOITCompositing.cpp

  ${SOURCE_DIR}/RenderPasses/PassToneMapping.cpp

  ${SOURCE_DIR}/RenderPasses/PassTAA.cpp

  ${SOURCE_DIR}/RenderPasses/PassPresent.cpp

  ${SOURCE_DIR}/Subsystems/FrameSubsystem.cpp
  ${SOURCE_DIR}/Subsystems/CameraSubsystem.cpp
  ${SOURCE_DIR}/Subsystems/LightsSubsystem.cpp
  ${SOURCE_DIR}/Subsystems/MaterialSubsystem.cpp
  ${SOURCE_DIR}/Subsystems/SkinSubsystem.cpp
  ${SOURCE_DIR}/Subsystems/MeshSubsystem.cpp
  ${SOURCE_DIR}/Subsystems/FogSubsystem.cpp
  ${SOURCE_DIR}/Subsystems/TexturingSubsystem.cpp
  ${SOURCE_DIR}/Components/LightData.cpp
  ${SOURCE_DIR}/Components/MeshSkin.cpp
  ${SOURCE_DIR}/Loader/SamplerLoader.cpp
  ${SOURCE_DIR}/Loader/TextureLoader.cpp
  ${SOURCE_DIR}/Loader/SparseTextureLoader.cpp
  ${SOURCE_DIR}/TextureBlurHelper.cpp
  ${SOURCE_DIR}/SparseTexture.cpp
  ${SOURCE_DIR}/SparseTexturePageCache.cpp
  ${SOURCE_DIR}/WorleyPerlinNoise.cpp
  ${SOURCE_DIR}/Material.cpp
  ${SOURCE_DIR}/Primitive.cpp
  ${SOURCE_DIR}/RenderBuffer.cpp
  ${SOURCE_DIR}/Renderer.cpp
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
  MSG::Material
  ${MSG_RENDERER_LIBS})

set_target_properties(
  MSG-Renderer-Implementation
  PROPERTIES FOLDER "MSG/Renderer")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()
