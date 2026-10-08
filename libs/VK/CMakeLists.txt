add_subdirectory(VKInvoke)
add_subdirectory(VKInstance)
add_subdirectory(VKMemoryAllocator)
add_subdirectory(VKBuffer)
add_subdirectory(VKImage)

add_library(MSG-VKUtils INTERFACE)
add_library(MSG::VKUtils ALIAS MSG-VKUtils)
target_link_libraries(MSG-VKUtils INTERFACE
  MSG::VKInvoke
  MSG::VKInstance
  MSG::VKMemoryAllocator
  MSG::VKBuffer
  MSG::VKImage)