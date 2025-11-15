set(HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/PO/Pair.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/PO/Reference.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/PO/SymetricMatrix.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/PO/Triangle.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/PO/Vertex.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/LodsGenerator.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/PrimitiveOptimizer.hpp)

set(SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/PO/Pair.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/PO/SymetricMatrix.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/PO/Triangle.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/PO/Vertex.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/LodsGenerator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/PrimitiveOptimizer.cpp)

add_library(MSG-PrimitiveOptimizer ${SOURCE_FILES} ${HEADER_FILES})
add_library(MSG::PrimitiveOptimizer ALIAS MSG-PrimitiveOptimizer)
target_include_directories(MSG-PrimitiveOptimizer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-PrimitiveOptimizer
  MSG::Shapes
  MSG::Mesh
  MSG::Tools
  MSG::Threads
  MSG::BiMap)

set_target_properties(
  MSG-PrimitiveOptimizer
  PROPERTIES FOLDER "MSG"
)

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()
