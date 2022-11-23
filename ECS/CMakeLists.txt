include(FetchContent)

set(TABGRAPH_ECS_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ECS/Common.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ECS/ComponentArray.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ECS/ComponentManager.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ECS/EntityManager.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ECS/Manager.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ECS/System.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ECS/SystemManager.hpp)

set(TABGRAPH_ECS_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ECS/Manager.cpp)

add_library(TabGraph-ECS ${TABGRAPH_ECS_SOURCE_FILES} ${TABGRAPH_ECS_HEADER_FILES})
add_library(TabGraph::ECS ALIAS TabGraph-ECS)
target_include_directories(TabGraph-ECS PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(TabGraph-ECS)

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
