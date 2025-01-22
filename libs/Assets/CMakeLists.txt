set(MSG_ASSETS_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Assets/Asset.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Assets/Parser.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Assets/Uri.hpp)

set(MSG_ASSETS_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Assets/Asset.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Assets/Parser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Assets/Uri.cpp)

set(MSG_PARSER_HEADER_FILES CACHE INTERNAL "")
set(MSG_PARSER_SOURCE_FILES CACHE INTERNAL "")

add_library(MSG-Assets ${MSG_ASSETS_HEADER_FILES} ${MSG_ASSETS_SOURCE_FILES} ${MSG_PARSER_HEADER_FILES} ${MSG_PARSER_SOURCE_FILES})
add_library(MSG::Assets ALIAS MSG-Assets)
target_include_directories(MSG-Assets PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-Assets MSG::Tools MSG::Scene)

add_subdirectory(Parsers)


set_target_properties(
  MSG-Assets
  PROPERTIES FOLDER "MSG/Assets"
)

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
