set(HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/VKBuffer.hpp)
set(SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/VKBuffer.cpp)

add_library(MSG-VKBuffer STATIC ${HEADER_FILES} ${SOURCE_FILES})
add_library(MSG::VKBuffer ALIAS MSG-VKBuffer)
target_include_directories(MSG-VKBuffer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-VKBuffer
  MSG::VKInstance
  MSG::VKMemoryAllocator)
set_target_properties(
  MSG-VKBuffer
  PROPERTIES FOLDER "MSG/VK")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()