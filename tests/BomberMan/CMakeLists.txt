cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0074 NEW)

set(CMAKE_DEBUG_TARGET_PROPERTIES INCLUDE_DIRECTORIES)
set(CMAKE_CXX_STANDARD 17)

set(HEADER_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Bomb.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/CrispyWall.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Flame.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Game.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/GameEntity.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Level.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Player.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Wall.hpp
    )

set(SOURCE_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Bomb.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CrispyWall.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Flame.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Game.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GameEntity.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Level.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Player.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Wall.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
	)

file(GLOB_RECURSE RESOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/res/*.*
	)

add_executable(Bomberman ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(Bomberman PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(Bomberman PUBLIC SDL2::Main TabGraph::Plugins TabGraph::TabGraph)

if(MSVC_IDE)
    set(CMAKE_DEBUG_POSTFIX "d")
    # Macro to preserve source files hierarchy in the IDE
    macro(GroupSources curdir)
        file(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/${curdir} ${CMAKE_CURRENT_SOURCE_DIR}/${curdir}/*)

        foreach(child ${children})
            if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${curdir}/${child})
                GroupSources(${curdir}/${child})
            else()
                string(REPLACE "/" "\\" groupname ${curdir})
                string(REPLACE "src" "Sources" groupname ${groupname})
                string(REPLACE "include" "Includes" groupname ${groupname})
                source_group(${groupname} FILES ${CMAKE_CURRENT_SOURCE_DIR}/${curdir}/${child})
            endif()
        endforeach()
    endmacro()

    # Run macro
    GroupSources(src)
    GroupSources(include)
endif()

foreach(item_absolute IN LISTS RESOURCE_FILES)
	file(RELATIVE_PATH item ${CMAKE_CURRENT_SOURCE_DIR} ${item_absolute})
    message(STATUS "Copy ${item} to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${item}")
    add_custom_command(TARGET Bomberman POST_BUILD
        #OUTPUT "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${item}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${item_absolute}" "$<TARGET_FILE_DIR:Bomberman>/${item}"
        #DEPENDS "${item_absolute}"
    )
endforeach()

