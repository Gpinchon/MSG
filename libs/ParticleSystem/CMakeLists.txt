Fetch_GLM()

set(MSG_PARENTING_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/ParticleSystem.hpp)

add_library(MSG-ParticleSystem INTERFACE ${MSG_PARENTING_HEADER_FILES})
add_library(MSG::ParticleSystem ALIAS MSG-ParticleSystem)
target_include_directories(MSG-ParticleSystem INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-ParticleSystem INTERFACE MSG::Component)

set_target_properties(
  MSG-ParticleSystem
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()
