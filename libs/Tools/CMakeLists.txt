
set(MSG_TOOLS_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/ArrayHasher.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/Base.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/Factorial.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/FPSCounter.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/Halton.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/Handle.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/HashCombine.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/LazyConstructor.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/MakeArrayHelper.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/ObjectCache.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/Pi.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/SequenceTable.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/ScopedTimer.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/StrongTypedef.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/TupleHasher.hpp)

set(MSG_TOOLS_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Tools/Base.cpp)

add_library(MSG-Tools ${MSG_TOOLS_SOURCE_FILES} ${MSG_TOOLS_HEADER_FILES})
add_library(MSG::Tools ALIAS MSG-Tools)
target_include_directories(MSG-Tools PUBLIC 
  "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-Tools glm::glm)

set_target_properties(
  MSG-Tools
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()
