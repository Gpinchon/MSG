include(FetchContent)

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

set(MSG_TOOLS_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/ApplyMax.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/ArrayHasher.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/Base.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/BRDFIntegration.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/CPUCompute.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/Debug.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/Factorial.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/FixedSizeMemoryPool.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/FixedSizeString.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/FPSCounter.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/Halton.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/Handle.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/HashCombine.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/LazyConstructor.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/LegendrePolynomial.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/MakeArrayHelper.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/ObjectCache.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/Pi.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/SequenceTable.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/SphericalHarmonics.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/ScopedTimer.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/SparseSet.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/StrongTypedef.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/ThreadPool.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/Tools.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/TupleHasher.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Tools/WorkerThread.hpp)

set(MSG_TOOLS_INLINE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/inline/MSG/Tools/CPUCompute.inl
  ${CMAKE_CURRENT_SOURCE_DIR}/inline/MSG/Tools/LegendrePolynomial.inl
  ${CMAKE_CURRENT_SOURCE_DIR}/inline/MSG/Tools/SphericalHarmonics.inl
  ${CMAKE_CURRENT_SOURCE_DIR}/inline/MSG/Tools/SphericalHarmonicsSample.inl)

set(MSG_TOOLS_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Tools/Base.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Tools/BRDFIntegration.cpp)

add_library(MSG-Tools ${MSG_TOOLS_SOURCE_FILES} ${MSG_TOOLS_HEADER_FILES} ${MSG_TOOLS_INLINE_FILES})
add_library(MSG::Tools ALIAS MSG-Tools)
target_include_directories(MSG-Tools PUBLIC 
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/inline")
target_link_libraries(MSG-Tools glm::glm gcem)

set_target_properties(
  MSG-Tools
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()
