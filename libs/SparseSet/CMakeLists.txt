set(HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/SparseSet.hpp)

set(INLINE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/inline/MSG/SparseSet.inl)

add_library(MSG-SparseSet INTERFACE ${HEADER_FILES} ${INLINE_FILES})
add_library(MSG::SparseSet ALIAS MSG-SparseSet)
target_include_directories(MSG-SparseSet INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/inline")

set_target_properties(
  MSG-SparseSet
  PROPERTIES FOLDER "MSG"
)

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()
