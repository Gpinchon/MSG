cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0074 NEW)

set(CMAKE_DEBUG_TARGET_PROPERTIES INCLUDE_DIRECTORIES)
set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
	)

file(GLOB_RECURSE RESOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/res/*.*
	)



add_executable(Scene ${SOURCE_FILES})

foreach(item_absolute IN LISTS RESOURCE_FILES)
	file(RELATIVE_PATH item ${CMAKE_CURRENT_SOURCE_DIR} ${item_absolute})
    message(STATUS "Copy ${item} to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${item}")
    add_custom_command(TARGET Scene POST_BUILD
        #OUTPUT "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${item}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${item_absolute}" "$<TARGET_FILE_DIR:Scene>/${item}"
        #DEPENDS "${item_absolute}"
    )
endforeach()

target_link_libraries(Scene PUBLIC SDL2::Main TabGraph::Plugins TabGraph::TabGraph)

#get_filename_component(TabGraph_SOURCE_DIR "../.." ABSOLUTE)
#message(${TabGraph_SOURCE_DIR})
#add_subdirectory(${TabGraph_SOURCE_DIR} "${PROJECT_SOURCE_DIR}/build/TabGraph")
#include(${TabGraph_SOURCE_DIR}/TabGraph-headers.cmake)

#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message("${_variableName}=${${_variableName}}")
#endforeach()

#include(${TabGraph_SOURCE_DIR}/TabGraphConfig.cmake)

#include(${TabGraph_SOURCE_DIR}/TabGraph-headers.cmake)
#include(ExternalProject)
#ExternalProject_Add(
#    TabGraph
#    SOURCE_DIR "${TabGraph_SOURCE_DIR}"
#    BINARY_DIR "${TabGraph_SOURCE_DIR}/build/TabGraph"
#    #CONFIGURE_COMMAND ""
#    #BUILD_COMMAND ""
#    #INSTALL_COMMAND ""
#    #UPDATE_COMMAND ""
#)
#ExternalProject_Get_Property(TabGraph SOURCE_DIR)
#ExternalProject_Get_Property(TabGraph BINARY_DIR)
#ExternalProject_Get_Property(TabGraph INSTALL_DIR)
#message("${SOURCE_DIR}")
#message("${BINARY_DIR}")
#message("${INSTALL_DIR}")



#file(GENERATE 
#    OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/includes.txt" 
#    CONTENT "$<TARGET_PROPERTY:TabGraph,INCLUDE_DIRECTORIES>\n"
#)

#get_target_property(TABGRAPH_INCLUDES TabGraph::LibTabGraph INCLUDE_DIRECTORIES)
#message("${TABGRAPH_INCLUDES}")

#ExternalProject_Get_Property(TabGraph install_dir)
#message("${TabGraph_SOURCE_DIR}/include")
#target_include_directories(Scene PRIVATE "${TabGraph_SOURCE_DIR}/include")
#add_library(TabGraph STATIC IMPORTED GLOBAL)
#add_dependencies(Scene TabGraph)
