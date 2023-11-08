include(FetchContent)

# Fetch GLM
FetchContent_Declare(
  GLM
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        0.9.9.8
)
FetchContent_MakeAvailable(GLM)
message(STATUS "Fetched GLM to ${glm_SOURCE_DIR}")
set(glm_DIR "${glm_SOURCE_DIR}/cmake/glm")
set_subdirectory_folder("3rdparty/GLM" ${glm_SOURCE_DIR})
# Fetch GLM

# Fetch GCEM
FetchContent_Declare(
  GCEM
  GIT_REPOSITORY https://github.com/kthohr/gcem.git
  GIT_TAG        v1.16.0
)
FetchContent_MakeAvailable(GCEM)
message(STATUS "Fetched GCEM to ${gcem_SOURCE_DIR}")
set_subdirectory_folder("3rdparty/GCEM" ${gcem_SOURCE_DIR})
# Fetch GCEM

set(TABGRAPH_TOOLS_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Tools/ApplyMax.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Tools/Base.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Tools/Debug.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Tools/Factorial.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Tools/FixedSizeMemoryPool.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Tools/FPSCounter.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Tools/Halton.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Tools/HashCombine.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Tools/LazyConstructor.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Tools/LegendrePolynomial.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Tools/ObjectCache.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Tools/Pi.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Tools/SequenceTable.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Tools/SphericalHarmonics.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Tools/ScopedTimer.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Tools/SparseSet.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Tools/ThreadPool.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Tools/Tools.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Tools/TupleHasher.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Tools/WorkerThread.hpp)

set(TABGRAPH_TOOLS_INLINE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/inline/Tools/LegendrePolynomial.inl
  ${CMAKE_CURRENT_SOURCE_DIR}/inline/Tools/SphericalHarmonics.inl
  ${CMAKE_CURRENT_SOURCE_DIR}/inline/Tools/SphericalHarmonicsSample.inl)

set(TABGRAPH_TOOLS_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Tools/Base.cpp)

add_library(TabGraph-Tools ${TABGRAPH_TOOLS_SOURCE_FILES} ${TABGRAPH_TOOLS_HEADER_FILES} ${TABGRAPH_TOOLS_INLINE_FILES})
add_library(TabGraph::Tools ALIAS TabGraph-Tools)
target_include_directories(TabGraph-Tools PUBLIC 
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/inline")
target_link_libraries(TabGraph-Tools glm::glm gcem)

set_target_properties(
  TabGraph-Tools
  PROPERTIES FOLDER "TabGraph"
)

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()
