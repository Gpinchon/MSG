include(FetchContent)

set(TABGRAPH_ECS_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ECS/EntityRef.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ECS/Registry.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ECS/ComponentTypeStorage.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ECS/EntityStorage.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ECS/View.hpp)

add_library(TabGraph-ECS ${TABGRAPH_ECS_HEADER_FILES})
add_library(TabGraph::ECS ALIAS TabGraph-ECS)
target_include_directories(TabGraph-ECS PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(TabGraph-ECS TabGraph::Tools)

set_target_properties(
  TabGraph-ECS
  PROPERTIES FOLDER "TabGraph"
)

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
