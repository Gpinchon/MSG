include(FetchContent)

macro(Fetch_SparseHash)
    if (NOT TARGET SparseHash)
        FetchContent_Declare(
            SPARSEHASH
            GIT_REPOSITORY  https://github.com/sparsehash/sparsehash-c11.git
            GIT_TAG         0c748d9528c06f8360b0edf6766d0f64a5f48034
        )
        FetchContent_Populate(SPARSEHASH)
        add_library(SparseHash INTERFACE)
        target_include_directories(SparseHash INTERFACE ${sparsehash_SOURCE_DIR})
    endif (NOT TARGET SparseHash)
endmacro()
