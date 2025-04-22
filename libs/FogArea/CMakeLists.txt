set(HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/FogArea.hpp)

set(SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/FogArea.cpp)

add_library(MSG-FogArea STATIC ${HEADER_FILES} ${SOURCE_FILES})
add_library(MSG::FogArea ALIAS MSG-FogArea)
target_include_directories(MSG-FogArea PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-FogArea PUBLIC
  MSG::Shapes
  MSG::Core)

set_target_properties(
  MSG-FogArea
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()
