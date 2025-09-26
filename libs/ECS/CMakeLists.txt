Fetch_GCEM()

set(HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/ECS/ApplyMax.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/ECS/ComponentTypeStorage.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/ECS/EntityRef.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/ECS/EntityWeakRef.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/ECS/EntityStorage.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/ECS/Registry.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/ECS/View.hpp)

set(INLINE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/inline/MSG/ECS/ComponentTypeStorage.inl
  ${CMAKE_CURRENT_SOURCE_DIR}/inline/MSG/ECS/EntityRef.inl
  ${CMAKE_CURRENT_SOURCE_DIR}/inline/MSG/ECS/Registry.inl
  ${CMAKE_CURRENT_SOURCE_DIR}/inline/MSG/ECS/View.inl)

add_library(MSG-ECS INTERFACE ${HEADER_FILES} ${INLINE_FILES})
add_library(MSG::ECS ALIAS MSG-ECS)
target_include_directories(MSG-ECS INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/inline")
target_link_libraries(MSG-ECS INTERFACE
  MSG::SparseSet
  MSG::MemoryPool
  gcem)

set_target_properties(
  MSG-ECS
  PROPERTIES FOLDER "MSG"
)

if(MSVC_IDE)
  GroupSources(include)
  GroupSources(inline)
endif()
