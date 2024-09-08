include(FetchContent)

# Fetch GOOGLE_TEST
macro(Fetch_GoogleTest)
  if (NOT TARGET gtest)
    FetchContent_Declare(
        GTEST
        GIT_REPOSITORY  https://github.com/google/googletest.git
        GIT_TAG         v1.15.2
    )
    FetchContent_MakeAvailable(GTEST)
    set_target_properties(
      gtest
      PROPERTIES FOLDER "3rdParty/GoogleTest")
    set_target_properties(
      gtest_main
      PROPERTIES FOLDER "3rdParty/GoogleTest")
    set_target_properties(
      gmock
      PROPERTIES FOLDER "3rdParty/GoogleTest")
    set_target_properties(
      gmock_main
      PROPERTIES FOLDER "3rdParty/GoogleTest")
  endif (NOT TARGET gtest)
endmacro()
