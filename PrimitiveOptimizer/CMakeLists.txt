Fetch_SparseHash()

set(HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/LodsGenerator.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/POBiMap.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/POPair.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/POReference.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/POStorage.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/POSymetricMatrix.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/POTriangle.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/POVertex.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SG/PrimitiveOptimizer.hpp)

set(SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/LodsGenerator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/POPair.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/POSymetricMatrix.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/POTriangle.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/POVertex.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/PrimitiveOptimizer.cpp)

add_library(TabGraph-PrimitiveOptimizer ${SOURCE_FILES} ${HEADER_FILES})
add_library(TabGraph::PrimitiveOptimizer ALIAS TabGraph-PrimitiveOptimizer)
target_include_directories(TabGraph-Tools PUBLIC 
  "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(TabGraph-PrimitiveOptimizer
  TabGraph::SG
  SparseHash)

set_target_properties(
  TabGraph-PrimitiveOptimizer
  PROPERTIES FOLDER "TabGraph"
)

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
