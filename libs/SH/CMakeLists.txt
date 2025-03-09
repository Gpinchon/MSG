include(FetchContent)

set(HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/SphericalHarmonics.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/SphericalHarmonicsSample.hpp)

set(INLINE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/inline/MSG/LegendrePolynomial.inl
  ${CMAKE_CURRENT_SOURCE_DIR}/inline/MSG/SphericalHarmonics.inl
  ${CMAKE_CURRENT_SOURCE_DIR}/inline/MSG/SphericalHarmonicsSample.inl)

add_library(MSG-SH INTERFACE ${HEADER_FILES} ${INLINE_FILES})
add_library(MSG::SH ALIAS MSG-SH)
target_include_directories(MSG-SH INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/inline")
target_link_libraries(MSG-SH INTERFACE MSG::Tools)

set_target_properties(
  MSG-SH
  PROPERTIES FOLDER "MSG"
)

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()
