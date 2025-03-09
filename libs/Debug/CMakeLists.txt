include(FetchContent)

set(HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Debug.hpp)

add_library(MSG-Debug INTERFACE ${HEADER_FILES})
add_library(MSG::Debug ALIAS MSG-Debug)
target_include_directories(MSG-Debug INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/include")

set_target_properties(
  MSG-Debug
  PROPERTIES FOLDER "MSG"
)

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()
