set(HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/VKImage.hpp)
set(SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/VKImage.cpp)

add_library(MSG-VKImage STATIC ${HEADER_FILES} ${SOURCE_FILES})
add_library(MSG::VKImage ALIAS MSG-VKImage)
target_include_directories(MSG-VKImage PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-VKImage
  MSG::VKInstance
  MSG::VKMemoryAllocator)
set_target_properties(
  MSG-VKImage
  PROPERTIES FOLDER "MSG/VK")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()