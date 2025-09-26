set(HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Module.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/ModulesLibrary.hpp)

set(INLINE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/inline/MSG/ModulesLibrary.inl)

add_library(MSG-Modules INTERFACE ${HEADER_FILES} ${INLINE_FILES})
add_library(MSG::Modules ALIAS MSG-Modules)
target_include_directories(MSG-Modules PUBLIC INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/inline")

set_target_properties(
  MSG-Modules
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  GroupSources(include)
  GroupSources(inline)
endif()
