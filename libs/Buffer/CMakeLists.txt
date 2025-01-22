set(MSG_BUFFER_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Buffer/Accessor.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Buffer/Iterator.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Buffer/View.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Buffer.hpp)

set(MSG_BUFFER_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Buffer/Accessor.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Buffer/View.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Buffer.cpp)

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
