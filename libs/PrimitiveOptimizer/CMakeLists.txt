Fetch_SparseHash()

set(HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/PO/BiMap.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/PO/Pair.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/PO/Reference.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/PO/SymetricMatrix.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/PO/Triangle.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/PO/Vertex.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/LodsGenerator.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/PrimitiveOptimizer.hpp)

set(SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/PO/Pair.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/PO/SymetricMatrix.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/PO/Triangle.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/PO/Vertex.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/LodsGenerator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/PrimitiveOptimizer.cpp)

add_library(MSG-PrimitiveOptimizer ${SOURCE_FILES} ${HEADER_FILES})
add_library(MSG::PrimitiveOptimizer ALIAS MSG-PrimitiveOptimizer)
target_include_directories(MSG-Tools PUBLIC 
  "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-PrimitiveOptimizer
  MSG::Core
  SparseHash)

set_target_properties(
  MSG-PrimitiveOptimizer
  PROPERTIES FOLDER "MSG"
)

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
