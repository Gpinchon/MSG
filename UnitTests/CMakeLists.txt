cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0074 NEW)

add_subdirectory_with_folder("UnitTests" Assets)
add_subdirectory_with_folder("UnitTests" SH)
add_subdirectory_with_folder("UnitTests" SG)

if(MSVC_IDE)
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
