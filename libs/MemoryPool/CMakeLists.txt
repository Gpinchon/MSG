set(HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/FixedSizeMemoryPool.hpp)

set(INLINE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/inline/MSG/FixedSizeMemoryPool.inl)

add_library(MSG-MemoryPool INTERFACE ${HEADER_FILES} ${INLINE_FILES})
add_library(MSG::MemoryPool ALIAS MSG-MemoryPool)
target_include_directories(MSG-MemoryPool INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/inline")

set_target_properties(
  MSG-MemoryPool
  PROPERTIES FOLDER "MSG"
)

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()
