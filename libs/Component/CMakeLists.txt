set(HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Component.hpp)

add_library(MSG-Component INTERFACE ${HEADER_FILES})
add_library(MSG::Component ALIAS MSG-Component)
target_include_directories(MSG-Component INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/include")

set_target_properties(
  MSG-Component
  PROPERTIES FOLDER "MSG"
)

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()
