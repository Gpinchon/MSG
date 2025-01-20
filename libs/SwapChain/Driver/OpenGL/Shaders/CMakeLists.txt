set(HEADERS_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/header)
set(STAGES_DIR   ${CMAKE_CURRENT_SOURCE_DIR}/stage/)
set(PROGRAMS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/program/)

set(SHADERS_PROGRAM_FILES ${SHADERS_PROGRAM_FILES}
  ${PROGRAMS_DIR}/SwapChain.json
  CACHE INTERNAL "")

set(SHADERS_HEADER_FILES ${SHADERS_HEADER_FILES}
  CACHE INTERNAL "")

set(SHADERS_STAGE_FILES ${SHADERS_STAGE_FILES}
  ${STAGES_DIR}/SwapChain.frag
  ${STAGES_DIR}/FullscreenTriangle.vert
  CACHE INTERNAL "")

add_library(MSG-SwapChain-Shaders INTERFACE
  ${SHADERS_PROGRAM_FILES} ${SHADERS_HEADER_FILES} ${SHADERS_STAGE_FILES})
add_library(MSG::SwapChain::Shaders ALIAS MSG-SwapChain-Shaders)
target_include_directories(MSG-SwapChain-Shaders PUBLIC INTERFACE ${HEADERS_DIR})
set_target_properties(
  MSG-SwapChain-Shaders
  PROPERTIES FOLDER "MSG")

source_group("Headers"  FILES ${SHADERS_HEADER_FILES})
source_group("Programs" FILES ${SHADERS_PROGRAM_FILES})
source_group("Stages"   FILES ${SHADERS_STAGE_FILES})
