set(TABGRAPH_SG_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Assets/Asset.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Assets/BinaryData.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Assets/Image.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Assets/Parser.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Assets/Uri.hpp)

set(TABGRAPH_SG_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Assets/Asset.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Assets/Image.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Assets/Parser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Assets/Uri.cpp)

add_library(TabGraph-Assets ${TABGRAPH_SG_SOURCE_FILES} ${TABGRAPH_SG_HEADER_FILES})
add_library(TabGraph::Assets ALIAS TabGraph-SG)
target_include_directories(TabGraph-Assets PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(TabGraph-Assets TabGraph::Tools TabGraph::SG)

set_target_properties(
  TabGraph-Assets
  PROPERTIES FOLDER "TabGraph"
)

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
