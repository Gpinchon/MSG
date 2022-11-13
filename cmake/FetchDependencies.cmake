include(FetchContent)
include(ExternalProject)
option(BUILD_EXECUTABLES "Build executables" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(BUILD_STATIC_LIBS "Build static libraries" ON)

FetchContent_Declare(
	GLM
	GIT_REPOSITORY https://github.com/g-truc/glm.git
	GIT_TAG        0.9.9.8
)

FetchContent_Declare(
	RAPIDJSON
	GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
	GIT_TAG        v1.1.0
)

FetchContent_Declare(
	ZLIB
	URL			https://www.zlib.net/zlib-1.2.11.tar.gz
	URL_HASH	SHA256=c3e5e9fdd5004dcb542feda5ee4f0ff0744628baf8ed2dd5d66f8ca1197cb1a1
)

FetchContent_Declare(
	PNG
	GIT_REPOSITORY https://github.com/glennrp/libpng.git
	GIT_TAG        v1.6.37
)

FetchContent_Declare(
	JPEG
	GIT_REPOSITORY https://github.com/csparker247/jpeg-cmake.git
	GIT_TAG        v1.1.0
)

FetchContent_Declare(
	SDL2
	GIT_REPOSITORY	https://github.com/libsdl-org/SDL
	GIT_TAG			release-2.0.14
)

FetchContent_Declare(
	SDL2_IMAGE
	GIT_REPOSITORY https://github.com/Gpinchon/SDL_image.git
)

FetchContent_Declare(
	GLEW
	GIT_REPOSITORY	https://github.com/Perlmint/glew-cmake.git
	GIT_TAG			glew-cmake-2.2.0
)

FetchContent_Declare(
	FASTC
	GIT_REPOSITORY	https://github.com/GammaUNC/FasTC.git
)

option(USE_GDAL "Use Gdal for terrains" OFF)
if (USE_GDAL)
	add_definitions(-DUSE_GDAL)
	FetchContent_Declare(
		GDAL
		URL			http://download.gisinternals.com/sdk/downloads/release-1911-x64-gdal-3-1-2-mapserver-7-6-1-libs.zip
		URL_HASH    MD5=EB26FA4603DFD9F84FC53D6678C7F7C1
	)
	FetchContent_GetProperties(GDAL)
	if(NOT GDAL_POPULATED)
		FetchContent_Populate(GDAL)
		message(STATUS "Fetched GDAL to ${gdal_SOURCE_DIR}")
		set(GDAL_ROOT PATH "${gdal_SOURCE_DIR}")
	endif()
	find_package(GDAL REQUIRED)
	add_dependencies(TabGraph GDAL::GDAL)
	target_link_libraries(TabGraph ${GDAL_LIBRARIES})
endif()

FetchContent_GetProperties(ZLIB)
if(NOT ZLIB_POPULATED)
	FetchContent_Populate(ZLIB)
	message(STATUS "Fetched ZLIB to ${zlib_SOURCE_DIR}")
	option(SKIP_INSTALL_ALL "" ON)
	add_subdirectory(${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
	set_target_properties(zlib PROPERTIES FOLDER "3rdparty/Zlib")
	set_target_properties(zlibstatic PROPERTIES FOLDER "3rdparty/Zlib")
	set_target_properties(minigzip PROPERTIES FOLDER "3rdparty/Zlib")
	set_target_properties(example PROPERTIES FOLDER "3rdparty/Zlib")
	set(ZLIB_FOUND 1)
	set(ZLIB_LIBRARY zlibstatic)
	set(ZLIB_LIBRARIES zlibstatic)
	set(ZLIB_INCLUDE_DIR "${zlib_SOURCE_DIR}" "${zlib_BINARY_DIR}")
endif()

FetchContent_GetProperties(PNG)
if(NOT PNG_POPULATED)
	FetchContent_Populate(PNG)
	message(STATUS "Fetched PNG to ${png_SOURCE_DIR}")
	option(PNG_SHARED "" OFF)
	option(PNG_TESTS "" OFF)
	option(PNG_BUILD_ZLIB "" ON)
	add_subdirectory(${png_SOURCE_DIR} ${png_BINARY_DIR})
	set_target_properties(png_static PROPERTIES FOLDER "3rdparty/PNG")
	set_target_properties(genfiles PROPERTIES FOLDER "3rdparty/PNG")
	set(PNG_FOUND 1)
	#set(PNG_LIBRARY png_static)
	set(PNG_LIBRARIES png_static)
	#set(PNG_PNG_INCLUDE_DIR "${png_SOURCE_DIR}" "${png_BINARY_DIR}")
	#set(PNG_INCLUDE_DIR "${png_SOURCE_DIR}" "${png_BINARY_DIR}")
	set(PNG_INCLUDE_DIRS "${png_SOURCE_DIR}" "${png_BINARY_DIR}")
endif()

FetchContent_GetProperties(JPEG)
if(NOT JPEG_POPULATED)
	FetchContent_Populate(JPEG)
	message(STATUS "Fetched JPEG to ${jpeg_SOURCE_DIR}")
	add_subdirectory(${jpeg_SOURCE_DIR} ${jpeg_BINARY_DIR})
	set_target_properties(jpeg_objs PROPERTIES FOLDER "3rdparty/Jpeg")
	set_target_properties(jpeg_static PROPERTIES FOLDER "3rdparty/Jpeg")
	set(JPEG_FOUND 1)
	set(JPEG_LIBRARIES jpeg_static)
	set(JPEG_INCLUDE_DIRS "${jpeg_SOURCE_DIR}/libjpeg" "${jpeg_BINARY_DIR}")
endif()

FetchContent_GetProperties(SDL2)
if(NOT SDL2_POPULATED)
	FetchContent_Populate(SDL2)
	message(STATUS "Fetched SDL2 to ${sdl2_SOURCE_DIR}")
	list(APPEND EXTRA_LIBS vcruntime)
	option(SDL_SHARED "" OFF)
	option(SDL_STATIC "" ON)
	add_subdirectory(${sdl2_SOURCE_DIR} ${sdl2_BINARY_DIR})
	add_library(SDL2::SDL2 ALIAS SDL2-static)
	add_library(SDL2::Core ALIAS SDL2-static)
	add_library(SDL2::Main ALIAS SDL2main)
	set_target_properties(SDL2-static PROPERTIES FOLDER "3rdparty/SDL2")
	set_target_properties(SDL2main PROPERTIES FOLDER "3rdparty/SDL2")
	set_target_properties(uninstall PROPERTIES FOLDER "3rdparty/SDL2")
	set(SDL2_FOUND 1)
	set(SDL2_LIBRARY SDL2::Core)
	set(SDL2_INCLUDE_DIR ${sdl2_SOURCE_DIR}/include)
endif()

FetchContent_GetProperties(SDL2_IMAGE)
if(NOT SDL2_IMAGE_POPULATED)
	FetchContent_Populate(SDL2_IMAGE)
	message(STATUS "Fetched SDL2_IMAGE to ${sdl2_image_SOURCE_DIR}")
	option(SDL2_IMAGE_SHARED "ENABLE SHARED BUILD" OFF)
	option(SDL2_IMAGE_BUILD_PNG "" ON)
	option(SDL2_IMAGE_BUILD_JPG "" ON)
	add_subdirectory(${sdl2_image_SOURCE_DIR} ${sdl2_image_BINARY_DIR})
	add_library(SDL2::Image ALIAS SDL2_image-static)
	set_target_properties(SDL2_image-static PROPERTIES FOLDER "3rdparty/SDL2")
	set(SDL2_IMAGE_FOUND 1)
	set(SDL2_IMAGE_LIBRARY SDL2::Image)
	set(SDL2_IMAGE_INCLUDE_DIR ${sdl2_image_SOURCE_DIR}/include)
	#set(SDL2_IMAGE_PATH "${sdl2_image_SOURCE_DIR}" CACHE STRING "Path to SDL2 Image library")
endif()

FetchContent_GetProperties(GLEW)
if(NOT GLEW_POPULATED)
	FetchContent_Populate(GLEW)
	message(STATUS "Fetched GLEW to ${glew_SOURCE_DIR}")
	option(ONLY_LIBS "Do not build executables" ON)
	option(glew-cmake_BUILD_STATIC "Build the static glew library" ON)
	option(glew-cmake_BUILD_SHARED "Build the shared glew library" OFF)
	#option(glew-cmake_BUILD_MULTI_CONTEXT "Build the multi context glew library" OFF)
	add_subdirectory(${glew_SOURCE_DIR} ${glew_BINARY_DIR})
	#add_library(GLEW::glew ALIAS libglew_shared)
	add_library(GLEW::glew_s ALIAS libglew_static)
	add_library(GLEW::GLEW ALIAS libglew_static)
	#target_link_libraries(libglew_static libglew_shared)
	#set_target_properties(libglew_shared PROPERTIES FOLDER "3rdparty")
	set_target_properties(libglew_static PROPERTIES FOLDER "3rdparty")
	set(GLEW_FOUND 1)
	set(GLEW_INCLUDE_DIR ${glew_SOURCE_DIR}/include)
	set(GLEW_LIBRARIES GLEW::glew GLEW::glew_s)
	#set(GLEW_ROOT PATH "${glew_SOURCE_DIR}")
	#set(CMAKE_PREFIX_PATH "${glew_SOURCE_DIR}")
	#if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
	#	set(CMAKE_LIBRARY_PATH "${glew_SOURCE_DIR}/lib/Release/Win32")
	#else()
	#	set(CMAKE_LIBRARY_PATH "${glew_SOURCE_DIR}/lib/Release/x64")
	#endif()
endif()

FetchContent_GetProperties(GLM)
if(NOT GLM_POPULATED)
	FetchContent_Populate(GLM)
	message(STATUS "Fetched GLM to ${glm_SOURCE_DIR}")
	#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${glm_SOURCE_DIR}/cmake/glm")
	set(glm_DIR "${glm_SOURCE_DIR}/cmake/glm")
	#set(GLM_ROOT PATH "${glm_SOURCE_DIR}")
endif()

FetchContent_GetProperties(RAPIDJSON)
if(NOT RAPIDJSON_POPULATED)
	FetchContent_Populate(RAPIDJSON)
	message(STATUS "Fetched RAPIDJSON to ${rapidjson_SOURCE_DIR}")
	set(RAPIDJSON_ROOT PATH "${rapidjson_SOURCE_DIR}")
endif()

ExternalProject_Add(
    RapidJSON
    SOURCE_DIR ${rapidjson_SOURCE_DIR}
    BINARY_DIR ${rapidjson_BINARY_DIR}
    TIMEOUT 10
    CMAKE_ARGS
        -DRAPIDJSON_BUILD_TESTS=OFF
        -DRAPIDJSON_BUILD_DOC=OFF
        -DRAPIDJSON_BUILD_EXAMPLES=OFF
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    UPDATE_COMMAND ""
)
# Prepare RapidJSON (RapidJSON is a header-only library)
set(RAPIDJSON_INCLUDE_DIR ${rapidjson_SOURCE_DIR}/include)

FetchContent_GetProperties(FASTC)
if(NOT FASTC_POPULATED)
	FetchContent_Populate(FASTC)
	message(STATUS "Fetched FASTC to ${fastc_SOURCE_DIR}")
	add_subdirectory(${fastc_SOURCE_DIR} ${fastc_BINARY_DIR})
	#[[THERE HAS TO BE A BETTER WAY TO DO THIS !]]
	set_target_properties(FasTCBase PROPERTIES FOLDER "3rdparty/FasTC/Libraries")
	set_target_properties(FasTCIO PROPERTIES FOLDER "3rdparty/FasTC/Libraries")
	set_target_properties(FasTCCore PROPERTIES FOLDER "3rdparty/FasTC/Libraries")
	set_target_properties(BPTCEncoder PROPERTIES FOLDER "3rdparty/FasTC/Libraries")
	set_target_properties(PVRTCEncoder PROPERTIES FOLDER "3rdparty/FasTC/Libraries")
	set_target_properties(DXTEncoder PROPERTIES FOLDER "3rdparty/FasTC/Libraries")
	set_target_properties(ETCEncoder PROPERTIES FOLDER "3rdparty/FasTC/Libraries")
	set_target_properties(ASTCEncoder PROPERTIES FOLDER "3rdparty/FasTC/Libraries")

	set_target_properties(tc PROPERTIES FOLDER "3rdparty/FasTC/Executables")
	set_target_properties(compare PROPERTIES FOLDER "3rdparty/FasTC/Executables")
	set_target_properties(decomp PROPERTIES FOLDER "3rdparty/FasTC/Executables")

	set_target_properties(gtest PROPERTIES FOLDER "3rdparty/FasTC/Tests")
	set_target_properties(gtest_main PROPERTIES FOLDER "3rdparty/FasTC/Tests")
	set_target_properties(Test_ASTCEncoder_IntegerEncoding PROPERTIES FOLDER "3rdparty/FasTC/Tests")
	set_target_properties(Test_PVRTCEncoder_Block PROPERTIES FOLDER "3rdparty/FasTC/Tests")
	set_target_properties(Test_PVRTCEncoder_Decompressor PROPERTIES FOLDER "3rdparty/FasTC/Tests")
	set_target_properties(Test_PVRTCEncoder_Image PROPERTIES FOLDER "3rdparty/FasTC/Tests")
	set_target_properties(Test_Base_Bits PROPERTIES FOLDER "3rdparty/FasTC/Tests")
	set_target_properties(Test_Base_Color PROPERTIES FOLDER "3rdparty/FasTC/Tests")
	set_target_properties(Test_Base_Image PROPERTIES FOLDER "3rdparty/FasTC/Tests")
	set_target_properties(Test_Base_Matrix PROPERTIES FOLDER "3rdparty/FasTC/Tests")
	set_target_properties(Test_Base_Pixel PROPERTIES FOLDER "3rdparty/FasTC/Tests")
	set_target_properties(Test_Base_Vector PROPERTIES FOLDER "3rdparty/FasTC/Tests")

	get_target_property(FASTCBASE_INCLUDES FasTCBase INCLUDE_DIRECTORIES)
	get_target_property(FASTCCORE_INCLUDES FasTCCore INCLUDE_DIRECTORIES)
endif()


#find_package(ZLIB REQUIRED)

#find_package(OpenGL REQUIRED)

#find_package(GLEW REQUIRED)

find_package(glm REQUIRED)
set(BUILD_EXECUTABLES ON)