set(HEADERS_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/header)
set(STAGES_DIR   ${CMAKE_CURRENT_SOURCE_DIR}/stage/)
set(PROGRAMS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/program/)

set(SHADERS_PROGRAM_FILES ${SHADERS_PROGRAM_FILES}
  ${PROGRAMS_DIR}/Fwd.json  
  ${PROGRAMS_DIR}/FwdMetRough_Blended.json
  ${PROGRAMS_DIR}/FwdMetRough_Opaque.json
  ${PROGRAMS_DIR}/FwdSpecGloss_Blended.json
  ${PROGRAMS_DIR}/FwdSpecGloss_Opaque.json
  ${PROGRAMS_DIR}/FwdMetRough_Blended_Unlit.json
  ${PROGRAMS_DIR}/FwdMetRough_Opaque_Unlit.json
  ${PROGRAMS_DIR}/FwdSpecGloss_Blended_Unlit.json
  ${PROGRAMS_DIR}/FwdSpecGloss_Opaque_Unlit.json
  ${PROGRAMS_DIR}/Compositing.json
  ${PROGRAMS_DIR}/CopyTexture.json
  ${PROGRAMS_DIR}/FogCulling.json
  ${PROGRAMS_DIR}/FogRendering.json
  ${PROGRAMS_DIR}/Present.json
  ${PROGRAMS_DIR}/Shadow_MetRough.json
  ${PROGRAMS_DIR}/Shadow_SpecGloss.json
  ${PROGRAMS_DIR}/Shadow_MetRough_Cube.json
  ${PROGRAMS_DIR}/Shadow_SpecGloss_Cube.json
  ${PROGRAMS_DIR}/Skybox.json
  ${PROGRAMS_DIR}/TemporalAccumulation.json
  ${PROGRAMS_DIR}/VTFSCulling.json
  CACHE INTERNAL "")

set(SHADERS_HEADER_FILES ${SHADERS_HEADER_FILES}
  ${HEADERS_DIR}/Bicubic.glsl  
  ${HEADERS_DIR}/Bindings.glsl
  ${HEADERS_DIR}/BRDF.glsl
  ${HEADERS_DIR}/Camera.glsl
  ${HEADERS_DIR}/Fog.glsl
  ${HEADERS_DIR}/FrameInfo.glsl
  ${HEADERS_DIR}/Functions.glsl
  ${HEADERS_DIR}/FwdLights.glsl
  ${HEADERS_DIR}/Lights.glsl
  ${HEADERS_DIR}/Material.glsl
  ${HEADERS_DIR}/MaterialInputs.glsl
  ${HEADERS_DIR}/MeshSkin.glsl
  ${HEADERS_DIR}/Random.glsl
  ${HEADERS_DIR}/SampleShadowMap.glsl
  ${HEADERS_DIR}/SphericalHarmonics.glsl
  ${HEADERS_DIR}/ToneMapping.glsl
  ${HEADERS_DIR}/Transform.glsl
  ${HEADERS_DIR}/Types.glsl
  ${HEADERS_DIR}/VTFS.glsl
  ${HEADERS_DIR}/VTFSInputs.glsl
  ${HEADERS_DIR}/VTFSLightSampling.glsl
  CACHE INTERNAL "")

set(SHADERS_STAGE_FILES ${SHADERS_STAGE_FILES}
  ${STAGES_DIR}/Compositing.frag
  ${STAGES_DIR}/CopyTexture.frag
  ${STAGES_DIR}/FogCulling.comp
  ${STAGES_DIR}/FogRendering.frag
  ${STAGES_DIR}/Forward.frag
  ${STAGES_DIR}/Forward.vert
  ${STAGES_DIR}/Present.frag
  ${STAGES_DIR}/Shadow.vert
  ${STAGES_DIR}/Shadow.frag
  ${STAGES_DIR}/Skybox.frag
  ${STAGES_DIR}/Skybox.vert
  ${STAGES_DIR}/TemporalAccumulation.frag
  ${STAGES_DIR}/FullscreenTriangle.vert
  ${STAGES_DIR}/VTFSCulling.comp
  CACHE INTERNAL "")

add_library(MSG-Renderer-Shaders INTERFACE
  ${SHADERS_PROGRAM_FILES} ${SHADERS_HEADER_FILES} ${SHADERS_STAGE_FILES})
add_library(MSG::Renderer::Shaders ALIAS MSG-Renderer-Shaders)
target_include_directories(MSG-Renderer-Shaders PUBLIC INTERFACE ${HEADERS_DIR})
set_target_properties(
  MSG-Renderer-Shaders
  PROPERTIES FOLDER "MSG/Renderer")

source_group("Headers"  FILES ${SHADERS_HEADER_FILES})
source_group("Programs" FILES ${SHADERS_PROGRAM_FILES})
source_group("Stages"   FILES ${SHADERS_STAGE_FILES})
