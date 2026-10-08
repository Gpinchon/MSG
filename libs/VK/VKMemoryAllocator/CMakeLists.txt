set(HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/VKMemoryAllocator.hpp)
set(SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/VKMemoryAllocator.cpp)

add_library(MSG-VKMemoryAllocator STATIC ${HEADER_FILES} ${SOURCE_FILES})
add_library(MSG::VKMemoryAllocator ALIAS MSG-VKMemoryAllocator)
target_include_directories(MSG-VKMemoryAllocator PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-VKMemoryAllocator
  MSG::VKInstance)
set_target_properties(
  MSG-VKMemoryAllocator
  PROPERTIES FOLDER "MSG/VK")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()