include(FetchContent)

set(TABGRAPH_ECS_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ECS/ComponentTypeStorage.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ECS/EntityRef.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ECS/EntityStorage.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ECS/Registry.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ECS/SparseSet.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ECS/View.hpp)

add_library(TabGraph-ECS INTERFACE ${TABGRAPH_ECS_HEADER_FILES})
add_library(TabGraph::ECS ALIAS TabGraph-ECS)
target_include_directories(TabGraph-ECS INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(TabGraph-ECS PUBLIC INTERFACE
  TabGraph::Tools)

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
