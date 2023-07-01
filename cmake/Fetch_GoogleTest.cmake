include(FetchContent)

# Fetch GOOGLE_TEST
macro(Fetch_GoogleTest)
  if (NOT TARGET GTest::GTest)
    FetchContent_Declare(
        GOOGLE_TEST
        GIT_REPOSITORY  https://github.com/google/googletest.git
        GIT_TAG         v1.13.0
    )
    option(INSTALL_GTEST "Install Google Test" OFF)
    FetchContent_MakeAvailable(GOOGLE_TEST)
    add_library(GTest::GTest INTERFACE IMPORTED)
    target_link_libraries(GTest::GTest INTERFACE gtest_main)
    set_subdirectory_folder("3rdparty/GoogleTest" ${google_test_SOURCE_DIR})
  endif (NOT TARGET GTest::GTest)
endmacro()
