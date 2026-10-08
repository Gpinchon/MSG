set(HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/VKTypedBuffer.hpp)
set(INLINE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/inline/MSG/VKTypedBuffer.inl)

add_library(MSG-VKTypedBuffer STATIC ${HEADER_FILES} ${INLINE_FILES})
add_library(MSG::VKTypedBuffer ALIAS MSG-VKTypedBuffer)
target_include_directories(MSG-VKTypedBuffer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-VKTypedBuffer
  MSG::VKInstance
  MSG::VKBuffer)
set_target_properties(
  MSG-VKTypedBuffer
  PROPERTIES FOLDER "MSG/VK")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()