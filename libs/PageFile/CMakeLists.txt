Fetch_GLM()

set(HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/PageFile.hpp)

set(SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/PageFile.cpp)

add_library(MSG-PageFile STATIC ${HEADER_FILES} ${SOURCE_FILES})
add_library(MSG::PageFile ALIAS MSG-PageFile)
target_include_directories(MSG-PageFile PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-PageFile)

set_target_properties(
  MSG-PageFile
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()
