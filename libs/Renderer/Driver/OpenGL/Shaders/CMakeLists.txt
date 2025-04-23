set(HEADERS_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/header)
set(STAGES_DIR   ${CMAKE_CURRENT_SOURCE_DIR}/stage/)
set(PROGRAMS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/program/)

set(SHADERS_PROGRAM_FILES ${SHADERS_PROGRAM_FILES}
  ${PROGRAMS_DIR}/Forward.json
  ${PROGRAMS_DIR}/DeferredFog.json
  ${PROGRAMS_DIR}/DeferredGeometry.json
  ${PROGRAMS_DIR}/DeferredIBL.json
  ${PROGRAMS_DIR}/DeferredShadows.json
  ${PROGRAMS_DIR}/DeferredSkybox.json
  ${PROGRAMS_DIR}/DeferredVTFS.json
  ${PROGRAMS_DIR}/Compositing.json
  ${PROGRAMS_DIR}/CopyTexture.json
  ${PROGRAMS_DIR}/FogIntegration.json
  ${PROGRAMS_DIR}/FogLightsInjection.json
  ${PROGRAMS_DIR}/FogParticipatingMedia.json
  ${PROGRAMS_DIR}/FogRendering.json
  ${PROGRAMS_DIR}/Present.json
  ${PROGRAMS_DIR}/Shadow.json
  ${PROGRAMS_DIR}/Skybox.json
  ${PROGRAMS_DIR}/TemporalAccumulation.json
  ${PROGRAMS_DIR}/VTFSCulling.json
  CACHE INTERNAL "")

set(SHADERS_HEADER_FILES ${SHADERS_HEADER_FILES}
  ${HEADERS_DIR}/Bicubic.glsl  
  ${HEADERS_DIR}/Bindings.glsl
  ${HEADERS_DIR}/BRDF.glsl
  ${HEADERS_DIR}/BRDFInputs.glsl
  ${HEADERS_DIR}/Camera.glsl
  ${HEADERS_DIR}/DeferredGBufferData.glsl
  ${HEADERS_DIR}/Fog.glsl
  ${HEADERS_DIR}/FogArea.glsl
  ${HEADERS_DIR}/FrameInfo.glsl
  ${HEADERS_DIR}/Functions.glsl
  ${HEADERS_DIR}/Lights.glsl
  ${HEADERS_DIR}/LightsIBLInputs.glsl
  ${HEADERS_DIR}/LightsShadowInputs.glsl
  ${HEADERS_DIR}/LightsVTFS.glsl
  ${HEADERS_DIR}/LightsVTFSInputs.glsl
  ${HEADERS_DIR}/LightsVTFSSampling.glsl
  ${HEADERS_DIR}/Material.glsl
  ${HEADERS_DIR}/MaterialInputs.glsl
  ${HEADERS_DIR}/MeshSkin.glsl
  ${HEADERS_DIR}/Random.glsl
  ${HEADERS_DIR}/SampleShadowMap.glsl
  ${HEADERS_DIR}/SDF.glsl
  ${HEADERS_DIR}/SphericalHarmonics.glsl
  ${HEADERS_DIR}/ToneMapping.glsl
  ${HEADERS_DIR}/Transform.glsl
  ${HEADERS_DIR}/Types.glsl
  CACHE INTERNAL "")

set(SHADERS_STAGE_FILES ${SHADERS_STAGE_FILES}
  ${STAGES_DIR}/Compositing.frag
  ${STAGES_DIR}/CopyTexture.frag
  ${STAGES_DIR}/FogIntegration.comp
  ${STAGES_DIR}/FogLightsInjection.comp
  ${STAGES_DIR}/FogParticipatingMedia.comp
  ${STAGES_DIR}/FogRendering.frag
  ${STAGES_DIR}/DeferredFog.frag
  ${STAGES_DIR}/DeferredGeometry.frag
  ${STAGES_DIR}/DeferredIBL.frag
  ${STAGES_DIR}/DeferredShadows.frag
  ${STAGES_DIR}/DeferredSkybox.frag
  ${STAGES_DIR}/DeferredVTFS.frag
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
