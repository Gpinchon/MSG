include(FetchContent)

set(HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/CPUCompute.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/ThreadPool.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/WorkerThread.hpp)

set(INLINE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/inline/MSG/CPUCompute.inl)

add_library(MSG-Threads INTERFACE ${HEADER_FILES})
add_library(MSG::Threads ALIAS MSG-Threads)
target_include_directories(MSG-Threads INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/include")

set_target_properties(
  MSG-Threads
  PROPERTIES FOLDER "MSG"
)

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()
