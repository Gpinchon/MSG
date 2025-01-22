set(MSG_BUFFER_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Buffer/Accessor.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Buffer/Iterator.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Buffer/View.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Buffer.hpp)

set(MSG_BUFFER_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Buffer/Accessor.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Buffer/View.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Buffer.cpp)

add_library(MSG-Buffer ${MSG_BUFFER_SOURCE_FILES} ${MSG_BUFFER_HEADER_FILES})
add_library(MSG::Buffer ALIAS MSG-Buffer)
target_include_directories(MSG-Buffer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-Buffer MSG::Core)

set_target_properties(
  MSG-Buffer
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
