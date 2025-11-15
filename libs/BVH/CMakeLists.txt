Fetch_SparseHash()

set(MSG_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/BVH.hpp)

set(MSG_INLINE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/inline/MSG/BVH.inl)

add_library(MSG-BVH INTERFACE ${MSG_HEADER_FILES} ${MSG_INLINE_FILES})
add_library(MSG::BVH ALIAS MSG-BVH)
target_include_directories(MSG-BVH INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/inline")
target_link_libraries(MSG-BVH INTERFACE
  MSG::BoundingVolume
  MSG::MemoryPool
  SparseHash)

set_target_properties(
  MSG-BVH
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  GroupSources(include)
  GroupSources(inline)
endif()
