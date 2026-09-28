Fetch_VulkanSDK()

set(HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/VKInstance.hpp)
set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/VKInstance.cpp)

add_library(MSG-VKInstance STATIC ${HEADER_FILES} ${SOURCE_FILES})
add_library(MSG::VKInstance ALIAS MSG-VKInstance)
target_include_directories(MSG-VKInstance PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-VKInstance
  MSG::Debug
  MSG::VKInvoke
  ${MSG_VK_LIBS})
set_target_properties(
  MSG-VKInstance
  PROPERTIES FOLDER "MSG/VK")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()