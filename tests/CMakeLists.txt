cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0074 NEW)

add_subdirectory_with_folder("MSG/UnitTests" PageFile)
add_subdirectory_with_folder("MSG/UnitTests" Assets)
add_subdirectory_with_folder("MSG/UnitTests" ECS)
add_subdirectory_with_folder("MSG/UnitTests" SH)
add_subdirectory_with_folder("MSG/UnitTests" SG)
add_subdirectory_with_folder("MSG/UnitTests" Renderer)
add_subdirectory_with_folder("MSG/UnitTests" SceneRenderer)
add_subdirectory_with_folder("MSG/UnitTests" Light)
add_subdirectory_with_folder("MSG/UnitTests" Sponza)

if(MSVC_IDE)
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
