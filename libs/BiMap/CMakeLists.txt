Fetch_SparseHash()

set(HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/BiMap.hpp)

set(INLINE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/inline/MSG/BiMap.inl)

add_library(MSG-BiMap INTERFACE ${HEADER_FILES} ${INLINE_FILES})
add_library(MSG::BiMap ALIAS MSG-BiMap)
target_include_directories(MSG-BiMap INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/inline")
target_link_libraries(MSG-BiMap INTERFACE SparseHash)

set_target_properties(
  MSG-BiMap
  PROPERTIES FOLDER "MSG"
)

if(MSVC_IDE)
  GroupSources(include)
  GroupSources(inline)
endif()
