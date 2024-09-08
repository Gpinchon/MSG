include(FetchContent)

macro(Fetch_GLM)
  FetchContent_Declare(
    GLM
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
  )
  FetchContent_MakeAvailable(GLM)
  message(STATUS "Fetched GLM to ${glm_SOURCE_DIR}")
  set(glm_DIR "${glm_SOURCE_DIR}/cmake/glm")
  set_subdirectory_folder("3rdparty/GLM" ${glm_SOURCE_DIR})
endmacro()
