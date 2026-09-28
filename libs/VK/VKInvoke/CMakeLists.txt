Fetch_VulkanSDK()

set(HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/VKInvoke.hpp)

add_library(MSG-VKInvoke INTERFACE ${HEADER_FILES})
add_library(MSG::VKInvoke ALIAS MSG-VKInvoke)
target_include_directories(MSG-VKInvoke INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-ECS INTERFACE ${MSG_VK_LIBS})
set_target_properties(
  MSG-VKInvoke
  PROPERTIES FOLDER "MSG/VK")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()